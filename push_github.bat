@echo off
chcp 65001 > nul
setlocal

echo ========================================================
echo   DONG BO VA UP CODE LEN GITHUB: huyhoang35419/BTL
echo   - Giu nguyen 100% lich su commit cua tat ca thanh vien
echo   - Xoa code cu tren repo va up ban chuan 100% trong may
echo ========================================================

set "PATH=C:\Program Files\Git\cmd;%PATH%"

set "REPO_URL=https://github.com/huyhoang35419/BTL.git"
set "PROJECT_DIR=%~dp0"
set "TEMP_DIR=%TEMP%\btl_git_temp_%RANDOM%"

echo [1/5] Dang clone lich su commit tu GitHub...
git clone "%REPO_URL%" "%TEMP_DIR%"
if %errorlevel% neq 0 (
    echo [!] Khong the clone repository. Vui long kiem tra ket noi mang hoac link GitHub.
    pause
    exit /b 1
)

echo [2/5] Dang chuyen lich su commit vao thu muc du an hien tai...
if exist "%PROJECT_DIR%.git" (
    attrib -h -r -s "%PROJECT_DIR%.git" 2>nul
    rd /s /q "%PROJECT_DIR%.git"
)

attrib -h -r -s "%TEMP_DIR%\.git" 2>nul
move "%TEMP_DIR%\.git" "%PROJECT_DIR%\" >nul
rd /s /q "%TEMP_DIR%" 2>nul

cd /d "%PROJECT_DIR%"

echo [3/5] Nhan dien nhanh mac dinh...
for /f "tokens=*" %%i in ('git branch --show-current') do set "CURRENT_BRANCH=%%i"
if "%CURRENT_BRANCH%"=="" set "CURRENT_BRANCH=main"
echo Nhanh hien tai: %CURRENT_BRANCH%

echo [4/5] Dang dong bo: xoa code cu tren repo va nap ban chuan moi...
git add -A
git commit -m "Cap nhat toan bo ma nguon chuan 100% WMS va cau hinh Vercel"

echo [5/5] Dang day len GitHub (giu nguyen toan bo commit history cu)...
git push origin %CURRENT_BRANCH%

if %errorlevel% neq 0 (
    echo [!] Thu push len nhanh master neu can...
    git push origin master
)

if %errorlevel% equ 0 (
    echo.
    echo ========================================================
    echo   DA CAP NHAT LEN GITHUB THANH CONG!
    echo   - 100% lich su commit cua cac thanh vien van duoc bao toan.
    echo   - Toan bo code cu tren repo da duoc thay the bang code moi nhat.
    echo ========================================================
) else (
    echo.
    echo [!] Push that bai. Co the ban can xac thuc/dang nhap GitHub tren may.
)

pause
