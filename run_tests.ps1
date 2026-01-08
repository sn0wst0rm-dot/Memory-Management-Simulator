Write-Host "--- Building Simulator ---" -ForegroundColor Cyan

g++ main.cpp src/cache/cache.cpp -I include -I src/cache -I include/vm -I src/vm -o final_sim.exe

if ($LASTEXITCODE -ne 0) {
    Write-Error "Compilation failed."
    exit 1
}

Write-Host "--- Running Integration Tests ---" -ForegroundColor Cyan
Write-Host "Input File: tests\full_simulation.txt"
Write-Host "-----------------------------------"

cmd /c "final_sim.exe < tests\full_simulation.txt"

Write-Host "`n-----------------------------------"
Write-Host "Test Sequence Complete." -ForegroundColor Green