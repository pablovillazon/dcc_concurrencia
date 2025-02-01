
# Obtener la cantidad total de memoria RAM
$totalRam = (Get-CimInstance Win32_PhysicalMemory | Measure-Object -Property capacity -Sum).Sum

# Monitorear el proceso "pasarela5.exe"
while($true) {
  $date = Get-Date -Format "yyyy-MM-dd HH:mm:ss"
  $process = Get-Process -Name "pasarela5" -ErrorAction SilentlyContinue
  if ($process) {
    $cpuTime = $process.CPU
    $memUsage = $process.WorkingSet64 / 1048576
    $date + ' > Proceso "pasarela5.exe" está en ejecución. CPU: ' + $cpuTime.ToString("#,0.000") + '%, Mem. usage: ' + $memUsage.ToString("N0") + 'MB'
  } else {
    $date + ' > Proceso "pasarela5.exe" no está en ejecución'
  }
  Start-Sleep -s 2
}
