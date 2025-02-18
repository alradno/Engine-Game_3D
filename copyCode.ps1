# Obtenemos la ruta del directorio donde se encuentra el script
$scriptDir = $PSScriptRoot

# Definimos el nombre y ruta completa del archivo de salida en el mismo directorio del script
$outputFile = Join-Path $scriptDir "generatedCopy.txt"

# Si el archivo de salida ya existe, lo borramos para que se sobreescriba
if (Test-Path $outputFile) {
    Remove-Item $outputFile -Force
}

# Array con los nombres de las carpetas a procesar (se asume que están en el mismo directorio que el script)
$folders = @("config", "include", "src", "shaders")

foreach ($folder in $folders) {
    # Construimos la ruta completa de la carpeta
    $folderPath = Join-Path $scriptDir $folder
    
    # Verificamos que la carpeta exista
    if (-Not (Test-Path $folderPath)) {
        Write-Host "La carpeta '$folderPath' no existe. Se omite."
        continue
    }
    
    # Obtenemos todos los archivos (excluyendo directorios) de la carpeta
    Get-ChildItem -Path $folderPath -File | ForEach-Object {
        $file = $_
        
        # Creamos un encabezado identificador para el contenido de cada archivo
        $separator = "-" * 60
        $header = "$separator`nArchivo: $($file.FullName)`n$separator"
        
        # Escribimos el encabezado en el archivo de salida
        Add-Content -Path $outputFile -Value $header
        
        # Leemos el contenido del archivo y lo agregamos
        $content = Get-Content -Path $file.FullName
        Add-Content -Path $outputFile -Value $content
        
        # Añadimos una línea en blanco para separar el contenido de diferentes archivos
        Add-Content -Path $outputFile -Value ""
    }
}

Write-Host "Se ha generado el archivo '$outputFile' con el contenido combinado."