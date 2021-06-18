[System.Threading.Thread]::CurrentThread.CurrentUICulture = "en-US" ; [System.Threading.Thread]::CurrentThread.CurrentCulture = "en-US"; $start_time = Get-Date -UFormat "%Y-%m-%d--%H-%M-%S"
$main = 'Main1.exe'
$baselines = @(
    '',
    'baseline\windows\baseline1.exe',
    'baseline\windows\baseline2.exe',
    'baseline\windows\baseline3.exe',
    'baseline\windows\baseline4.exe',
    'baseline\windows\baseline5.exe'
)
$player = @('Player.exe', $baselines[5])
$start_time += "--PvsB5" #name of test

for ($i = 1; $i -le 6; $i++) {
    $room_base = Join-Path 'room' $start_time
    $room_dir = Join-Path $room_base ('room' + $i)

    New-Item -ItemType directory -Path $room_dir
    Copy-Item $main $room_dir
    Copy-Item $player[0] $room_dir
    Copy-Item $player[1] $room_dir

    Start-Process -WindowStyle Hidden -WorkingDirectory $room_dir -FilePath (Join-Path $room_dir $main) -ArgumentList (Get-ChildItem -Path $player[($i + 1) % 2] -Name), (Get-ChildItem -Path $player[$i % 2] -Name) -RedirectStandardOutput (Join-Path $room_base ("Log" + $i + ".txt"))
}