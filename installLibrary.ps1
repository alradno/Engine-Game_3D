<#
.SYNOPSIS
  Instala una librería a partir de un ZIP, actualiza los archivos de configuración
  (CMakeLists.txt y .vscode\c_cpp_properties.json) para incluir la librería instalada,
  y realiza backup de dichos archivos con un nombre fijo.

.DESCRIPTION
  El script realiza lo siguiente:
    1. Descomprime el archivo ZIP de la librería.
    2. Si se detecta un CMakeLists.txt (en la raíz o en subdirectorios), se asume que la
       librería debe compilarse. Se crea la carpeta "build_msvc", se configura y se compila
       la librería con CMake (usando Visual Studio 17 2022 y -DCMAKE_INSTALL_PREFIX=".\install").  
       Si no se encuentra un CMakeLists.txt (por ejemplo, en el caso de la librería glad),
       se omite la compilación y se copian directamente los archivos.
    3. Se determinan las carpetas de interés ("include", "lib", "bin" y "src") y se copian
       a la ruta:
         $PSScriptRoot\libs\<nombreLibreria>
       Para el caso de librerías como stb, si no se encuentra una carpeta "include", se
       detecta si el directorio extraído contiene un único subdirectorio (por ejemplo, "stb-master")
       y se copian, de forma recursiva, todos los archivos *.h desde ese directorio a
         $PSScriptRoot\libs\<nombreLibreria>\include.
       Si ya existe la carpeta destino, se asume que la librería ya está instalada y se omite el proceso.
    4. Se actualizan los archivos de configuración:
         - **CMakeLists.txt**: Se agrega la ruta de include al bloque target_include_directories(Toxic PRIVATE …)
           y, en caso de existir archivos .lib en la carpeta lib, se agregan al bloque target_link_libraries(Toxic PRIVATE …)
           sin duplicar los bloques existentes.
         - **.vscode\c_cpp_properties.json**: Se añade la ruta de include de la librería en la propiedad
           includePath de cada configuración.
    5. Antes de modificar cada uno de estos archivos se crea un backup en el mismo directorio con
       el formato:  
           backup_<nombreArchivoOriginal>_<nombreLibreria>

.PARAMETER ZipPath
  Ruta completa al archivo ZIP de la librería.

.EXAMPLE
  .\InstalarLibreria.ps1 -ZipPath "C:\Users\Alberto\Downloads\stb.zip"
#>

param(
    [Parameter(Mandatory = $true, HelpMessage = "Ruta completa al archivo ZIP de la librería")]
    [string]$ZipPath
)

##############################################
# Función para realizar backup con nombre fijo #
##############################################
function Backup-File {
    param(
        [Parameter(Mandatory = $true)]
        [string]$FilePath,
        [Parameter(Mandatory = $true)]
        [string]$libName
    )
    if (-not (Test-Path $FilePath)) {
        return
    }
    $directory = Split-Path $FilePath -Parent
    $fileName = Split-Path $FilePath -Leaf
    $backupFileName = "backup_" + $fileName + "_" + $libName
    $backupFilePath = Join-Path $directory $backupFileName
    if (Test-Path $backupFilePath) {
        Write-Host "El backup '$backupFileName' ya existe en $directory. Se omite su creación."
    }
    else {
        Copy-Item -Path $FilePath -Destination $backupFilePath -Force
        Write-Host "Se ha creado el backup '$backupFileName' en $directory."
    }
}

#####################################
# Parte 1: Instalación de la librería #
#####################################

# Directorio base del workspace (donde se encuentra el script)
$WorkspaceFolder = $PSScriptRoot

# Verificar que el archivo ZIP existe
if (-not (Test-Path $ZipPath)) {
    Write-Error "El archivo ZIP no existe: $ZipPath"
    exit 1
}

# Obtener carpeta y nombre base (sin extensión) del ZIP
$zipDirectory = Split-Path $ZipPath -Parent
$baseName = [System.IO.Path]::GetFileNameWithoutExtension($ZipPath)
$ExtractFolder = Join-Path $zipDirectory $baseName

# Si la carpeta de extracción ya existe, se elimina para comenzar limpio
if (Test-Path $ExtractFolder) {
    Write-Host "La carpeta '$ExtractFolder' ya existe. Se elimina para comenzar una nueva instalación..."
    Remove-Item $ExtractFolder -Recurse -Force
}

# Descomprimir el ZIP
Write-Host "Descomprimiendo '$ZipPath' en '$ExtractFolder'..."
Expand-Archive -Path $ZipPath -DestinationPath $ExtractFolder

# Verificar si existe algún CMakeLists.txt en la carpeta extraída (o en sus subdirectorios)
$cmakeLists = Get-ChildItem -Path $ExtractFolder -Recurse -Filter "CMakeLists.txt" -File

if ($cmakeLists.Count -gt 0) {
    # Se encontró al menos un CMakeLists.txt; se procede con la compilación.
    $sourceDir = $cmakeLists[0].DirectoryName
    Write-Host "Se encontró CMakeLists.txt en: $sourceDir"
    
    # Crear la carpeta build_msvc para la compilación.
    $buildFolder = Join-Path $ExtractFolder "build_msvc"
    if (-not (Test-Path $buildFolder)) {
        Write-Host "Creando carpeta 'build_msvc'..."
        New-Item -ItemType Directory -Path $buildFolder | Out-Null
    }
    
    # Entrar a la carpeta build_msvc
    Set-Location $buildFolder
    
    # Resolver el directorio fuente (ruta absoluta)
    $sourceDirResolved = (Resolve-Path $sourceDir).Path
    
    # Ejecutar CMake para configurar la compilación
    Write-Host "Ejecutando CMake para configurar la compilación..."
    $cmakeArgs = @('-G', 'Visual Studio 17 2022', '-DCMAKE_INSTALL_PREFIX=.\install', $sourceDirResolved)
    Write-Host "Comando: cmake $($cmakeArgs -join ' ')"
    & cmake @cmakeArgs
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Error durante la configuración de CMake."
        exit $LASTEXITCODE
    }
    
    # ----- MODIFICACIÓN: Preguntar al usuario la configuración de compilación -----
    $buildChoice = Read-Host "Seleccione la configuración de compilación: (1) Release, (2) Debug (default: Release)"
    if ($buildChoice -eq "2") {
        $buildConfigName = "Debug"
    }
    else {
        $buildConfigName = "Release"
    }
    Write-Host "Construyendo e instalando la librería en modo $buildConfigName..."
    # --------------------------------------------------------------------------------
    
    # Compilar e instalar la librería en la configuración elegida
    & cmake --build . --config $buildConfigName --target INSTALL
    if ($LASTEXITCODE -ne 0) {
        Write-Error "Error durante la compilación/instalación de la librería."
        exit $LASTEXITCODE
    }
    
    # Volver a la carpeta raíz de la extracción
    Set-Location $ExtractFolder
}
else {
    Write-Host "No se encontró CMakeLists.txt. Se omitirá la compilación y se copiarán directamente los archivos."
}

# Definir las carpetas de interés para copiar ("include", "lib", "bin" y "src")
$foldersToSearch = @("include", "lib", "bin", "src")
$foundFolders = @{}
foreach ($folder in $foldersToSearch) {
    $found = Get-ChildItem -Path $ExtractFolder -Directory -Recurse | Where-Object { $_.Name -eq $folder }
    $foundFolders[$folder] = $found
    Write-Host "Se encontraron $($found.Count) carpeta(s) '$folder' en el ZIP."
}

# Determinar el nombre limpio de la librería (se asume formato nombre-version; se toma la parte anterior al guión)
$rawLibName = Split-Path $ExtractFolder -Leaf
$cleanLibName = ($rawLibName -split '-')[0]
Write-Host "Nombre de la librería a instalar: '$cleanLibName'"

# Definir la carpeta destino en el workspace: $PSScriptRoot\libs\<nombreLibreria>
$libsFolder = Join-Path $WorkspaceFolder "libs"
if (-not (Test-Path $libsFolder)) {
    Write-Host "Creando la carpeta 'libs' en el workspace..."
    New-Item -ItemType Directory -Path $libsFolder | Out-Null
}
$destLibFolder = Join-Path $libsFolder $cleanLibName

# Si la librería ya está instalada, se omite la instalación y actualización (sin crear backups)
if (Test-Path $destLibFolder) {
    Write-Host "La librería '$cleanLibName' ya está instalada en '$destLibFolder'."
    Write-Host "No se realizarán cambios ni backups."
    exit 0
}
else {
    Write-Host "La librería '$cleanLibName' no está instalada. Se procederá con la copia de archivos."
    New-Item -ItemType Directory -Path $destLibFolder | Out-Null
}

# Copiar el contenido de cada carpeta encontrada a la carpeta destino.
# Tratamos el caso especial de "include": si no se encontró ninguna carpeta "include",
# se busca un posible directorio raíz de cabeceras.
foreach ($folderName in $foldersToSearch) {
    if ($folderName -eq "include" -and $foundFolders[$folderName].Count -eq 0) {
        # Si no se encontró una carpeta "include", se intenta determinar el directorio que contiene los .h.
        $subdirs = Get-ChildItem -Path $ExtractFolder -Directory
        if ($subdirs.Count -eq 1) {
            $headerSource = $subdirs[0].FullName
            Write-Host "No se encontró carpeta 'include'. Se detectó un único subdirectorio: '$headerSource'."
        }
        else {
            $headerSource = $ExtractFolder
            Write-Host "No se encontró carpeta 'include'. Se usará el directorio raíz: '$headerSource'."
        }
        # Buscar recursivamente archivos .h en el directorio determinado.
        $headerFiles = Get-ChildItem -Path $headerSource -Filter *.h -File -Recurse
        if ($headerFiles.Count -gt 0) {
            $destSubfolder = Join-Path $destLibFolder "include"
            if (-not (Test-Path $destSubfolder)) {
                New-Item -ItemType Directory -Path $destSubfolder | Out-Null
            }
            Write-Host "Copiando los archivos .h desde '$headerSource' a '$destSubfolder'..."
            foreach ($file in $headerFiles) {
                Write-Host "Copiando '$($file.FullName)' a '$destSubfolder'..."
                Copy-Item -Path $file.FullName -Destination $destSubfolder -Force
            }
        }
        else {
            Write-Host "No se encontraron archivos .h en '$headerSource'."
        }
    }
    else {
        foreach ($dir in $foundFolders[$folderName]) {
            $destSubfolder = Join-Path $destLibFolder $folderName
            if (-not (Test-Path $destSubfolder)) {
                New-Item -ItemType Directory -Path $destSubfolder | Out-Null
            }
            Write-Host "Copiando contenido de '$($dir.FullName)' a '$destSubfolder'..."
            Copy-Item -Path (Join-Path $dir.FullName '*') -Destination $destSubfolder -Recurse -Force
        }
    }
}

#########################################################
# Parte 2: Actualizar archivos de configuración (con backup) #
#########################################################

# Rutas a los archivos de configuración en el workspace
$cmakelistsPath = Join-Path $WorkspaceFolder "CMakeLists.txt"
$vscodePropertiesPath = Join-Path $WorkspaceFolder ".vscode\c_cpp_properties.json"

# Nueva ruta de include a agregar (se asume la estructura actual de CMakeLists.txt)
$newIncludeCMake = '${CMAKE_SOURCE_DIR}/libs/' + $cleanLibName + '/include'
$newIncludeVSCode = '${workspaceFolder}/libs/' + $cleanLibName + '/include'

# --- Actualización de CMakeLists.txt ---
if (Test-Path $cmakelistsPath) {
    Write-Host "Actualizando 'CMakeLists.txt'..."
    # Realizar backup con nombre fijo
    Backup-File -FilePath $cmakelistsPath -libName $cleanLibName

    $cmakelistsContent = Get-Content $cmakelistsPath -Raw

    # Actualizar el bloque target_include_directories(Toxic PRIVATE ...)

    $regexTargetInclude = [regex]::new('target_include_directories\s*\(\s*Toxic\s+PRIVATE\s*(?<content>.*?)\)', [System.Text.RegularExpressions.RegexOptions]::Singleline)
    $matchTargetInclude = $regexTargetInclude.Match($cmakelistsContent)
    if ($matchTargetInclude.Success) {
        if ($matchTargetInclude.Groups["content"].Value -notmatch [regex]::Escape($newIncludeCMake)) {
            $oldContent = $matchTargetInclude.Groups["content"].Value.TrimEnd()
            $newContent = $oldContent + "`n    " + $newIncludeCMake
            $newBlock = "target_include_directories(Toxic PRIVATE" + "`n    " + $newContent + "`n)"
            $cmakelistsContent = $regexTargetInclude.Replace($cmakelistsContent, $newBlock, 1)
            Write-Host "Se agregó '$newIncludeCMake' al bloque target_include_directories."
        }
        else {
            Write-Host "La ruta de include '$newIncludeCMake' ya está presente en target_include_directories."
        }
    }
    else {
        $cmakelistsContent += "`n" + "target_include_directories(Toxic PRIVATE" + "`n    " + $newIncludeCMake + "`n)" + "`n"
        Write-Host "No se encontró bloque target_include_directories. Se añadió uno nuevo al final."
    }

    # Actualizar el bloque target_link_libraries(Toxic PRIVATE ...)
    $libsToAdd = @()
    $libFolderPath = Join-Path $destLibFolder "lib"
    if (Test-Path $libFolderPath) {
        $libFiles = Get-ChildItem -Path $libFolderPath -Filter *.lib
        if ($libFiles.Count -gt 0) {
            foreach ($file in $libFiles) {
                $relativeLib = '${CMAKE_SOURCE_DIR}/libs/' + $cleanLibName + '/lib/' + $file.Name
                if ($cmakelistsContent -notmatch [regex]::Escape($relativeLib)) {
                    $libsToAdd += $relativeLib
                }
            }
        }
    }
    if ($libsToAdd.Count -gt 0) {
        $regexTargetLink = [regex]::new('target_link_libraries\s*\(\s*Toxic\s+PRIVATE\s*(?<content>.*?)\)', [System.Text.RegularExpressions.RegexOptions]::Singleline)
        $matchTargetLink = $regexTargetLink.Match($cmakelistsContent)
        if ($matchTargetLink.Success) {
            $oldContent = $matchTargetLink.Groups["content"].Value.TrimEnd()
            $lines = $oldContent -split "\r?\n" | ForEach-Object { $_.Trim() } | Where-Object { $_ -ne "" }
            $libsToAppend = @()
            foreach ($lib in $libsToAdd) {
                if (-not ($lines -contains $lib)) {
                    $libsToAppend += $lib
                }
            }
            if ($libsToAppend.Count -gt 0) {
                $newContent = $oldContent + "`n    " + ($libsToAppend -join "`n    ")
                $newBlock = "target_link_libraries(Toxic PRIVATE" + "`n    " + $newContent + "`n)"
                $cmakelistsContent = $regexTargetLink.Replace($cmakelistsContent, $newBlock, 1)
                Write-Host "Se agregaron las librerías: $($libsToAppend -join ', ') al bloque target_link_libraries."
            }
            else {
                Write-Host "Las librerías ya están presentes en target_link_libraries."
            }
        }
        else {
            $newLinkBlock = "target_link_libraries(Toxic PRIVATE" + "`n    " + ($libsToAdd -join "`n    ") + "`n)"
            $cmakelistsContent += "`n`n" + $newLinkBlock + "`n"
            Write-Host "Se añadió un nuevo bloque target_link_libraries con: $($libsToAdd -join ', ')."
        }
    }
    else {
        Write-Host "No se encontraron archivos .lib para agregar a target_link_libraries."
    }

    Set-Content -Path $cmakelistsPath -Value $cmakelistsContent -Encoding UTF8
}
else {
    Write-Warning "No se encontró 'CMakeLists.txt' en el workspace ($cmakelistsPath)."
}

# --- Actualización de .vscode\c_cpp_properties.json ---
if (Test-Path $vscodePropertiesPath) {
    Write-Host "Actualizando '.vscode\c_cpp_properties.json'..."
    Backup-File -FilePath $vscodePropertiesPath -libName $cleanLibName

    $jsonText = Get-Content $vscodePropertiesPath -Raw
    try {
        $jsonObj = $jsonText | ConvertFrom-Json
    }
    catch {
        Write-Error "Error al parsear '$vscodePropertiesPath' como JSON."
        exit 1
    }
    
    $modified = $false
    foreach ($config in $jsonObj.configurations) {
        if ($config.includePath -notcontains $newIncludeVSCode) {
            $config.includePath += $newIncludeVSCode
            $modified = $true
            Write-Host "Se agregó '$newIncludeVSCode' a includePath de la configuración '$($config.name)'."
        }
        else {
            Write-Host "La ruta '$newIncludeVSCode' ya está presente en includePath de la configuración '$($config.name)'."
        }
    }
    if ($modified) {
        $newJson = $jsonObj | ConvertTo-Json -Depth 10
        Set-Content -Path $vscodePropertiesPath -Value $newJson -Encoding UTF8
    }
}
else {
    Write-Warning "No se encontró el archivo '.vscode\c_cpp_properties.json' en $WorkspaceFolder."
}

Write-Host "Instalación y actualización de archivos completadas exitosamente."