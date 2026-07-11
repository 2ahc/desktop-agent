param(
	[string]$QtPath = "D:\Qt\6.8.3\msvc2022_64",
	[string]$BuildDir = "build",
	[ValidateSet('Debug','Release')][string]$Config = "Release",
	[string]$Arch = "x64",
	[switch]$UseVcpkg = $false,
	[string]$VcpkgRoot = "vcpkg",
	[string]$VcpkgTriplet = "x64-windows",
	[switch]$ExportCmakePrefixPath = $false
)

# 如果环境变量 CMAKE_PREFIX_PATH 存在则优先使用
if ($Env:CMAKE_PREFIX_PATH) {
	$prefix = $Env:CMAKE_PREFIX_PATH
} else {
	$prefix = $QtPath
}

$scriptDir = Split-Path -Parent $MyInvocation.MyCommand.Path
Push-Location $scriptDir

# 创建输出目录
if (-Not (Test-Path $BuildDir)) { New-Item -ItemType Directory -Path $BuildDir | Out-Null }

Write-Host "Configuring with CMAKE_PREFIX_PATH=$prefix, Arch=$Arch, Config=$Config"

$cmakeArgs = @(
	"-S", ".",
	"-B", $BuildDir,
	"-A", $Arch,
	"-DCMAKE_PREFIX_PATH=$prefix"
)

# 可选：通过 vcpkg 自动安装并使用 vcpkg 的 toolchain
if ($UseVcpkg) {
	$vcpkgPath = Join-Path $scriptDir $VcpkgRoot
	if (-Not (Test-Path $vcpkgPath)) {
		Write-Host "vcpkg 未找到，正在克隆到 $vcpkgPath 并引导安装..."
		git clone https://github.com/microsoft/vcpkg.git $vcpkgPath
		if ($LASTEXITCODE -ne 0) { Write-Error "git clone vcpkg 失败"; exit $LASTEXITCODE }
		Push-Location $vcpkgPath
		.\bootstrap-vcpkg.bat
		if ($LASTEXITCODE -ne 0) { Write-Error "vcpkg bootstrap 失败"; Pop-Location; exit $LASTEXITCODE }
		Pop-Location
	}

	$toolchain = Join-Path $vcpkgPath "scripts\buildsystems\vcpkg.cmake"
	if (-Not (Test-Path $toolchain)) { Write-Error "找不到 vcpkg toolchain: $toolchain"; exit 1 }
	$cmakeArgs += "-DCMAKE_TOOLCHAIN_FILE=$toolchain"
	$cmakeArgs += "-DVCPKG_TARGET_TRIPLET=$VcpkgTriplet"
}

# 可选：导出 CMAKE_PREFIX_PATH 到当前进程环境，便于后续工具识别
if ($ExportCmakePrefixPath) {
	Write-Host "导出 CMAKE_PREFIX_PATH=$prefix 到当前进程环境"
	$Env:CMAKE_PREFIX_PATH = $prefix
}

cmake @cmakeArgs
if ($LASTEXITCODE -ne 0) {
	Write-Error "CMake configure failed (exit $LASTEXITCODE)"
	Pop-Location
	exit $LASTEXITCODE
}

cmake --build $BuildDir --config $Config
$exitCode = $LASTEXITCODE
Pop-Location
exit $exitCode
