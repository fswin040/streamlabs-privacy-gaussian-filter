# Streamlabs Privacy Gaussian Filter

給 Windows 版 Streamlabs Desktop 使用的原生隱私高斯模糊濾鏡。它能模糊整個來源，或只模糊以百分比指定的矩形區域，適合遮住畫面中的文字、人物與其他敏感資訊。

> [!IMPORTANT]
> 這是非官方社群專案，與 Streamlabs、OBS Project 無從屬或背書關係。目前只驗證 **Streamlabs Desktop 1.21.4（Windows x64，內建 `obs.dll` 31.1.2）**。Streamlabs 更新後可能不相容或刪除外掛，請先等本專案發布相容版本。

## 下載與安裝

1. 從 [GitHub Releases](../../releases) 下載 `MotionFrostedGlass-Setup-0.1.6.exe`。
2. 完全關閉 Streamlabs Desktop。
3. 以系統管理員身分執行安裝程式。
4. Windows SmartScreen 可能因安裝檔尚未購買程式碼簽章而警告；請核對下方 SHA-256 後，再決定是否選擇「其他資訊」→「仍要執行」。
5. 在 Streamlabs 的來源上開啟「濾鏡」，加入 **純高斯模糊**。部分介面可能因相容性識別碼而顯示 **Shader**。

安裝程式只接受 Streamlabs Desktop 1.21.4 與 `obs.dll` 31.1.2，版本不符時會停止，不會覆寫 `obs.dll`、Streamlabs 執行檔或場景檔。

## 功能

- 整個來源或自訂矩形區域
- 模糊強度
- 低、中、高三種品質
- 區域位置與大小（百分比）
- 一鍵重設預設值
- 強度為零時略過模糊運算
- 只處理含安全邊距的指定區域，降低 GPU 負擔

此版本專注於真正的高斯模糊，沒有動畫、玻璃透明度、彩色光效、邊框或顆粒效果。

## 相容性限制

Streamlabs Desktop 1.21.4 對可顯示的濾鏡類型有固定清單。本外掛使用 `shader_filter` 相容性識別碼，讓濾鏡能出現在 Streamlabs 介面中。這是非官方相容方式：

- 不要同時安裝其他也使用相同識別碼的外掛，例如某些 OBS Shader Filter 版本。
- Streamlabs 更新可能改變內部 OBS ABI、移除外掛檔案或造成無法啟動。
- 更新 Streamlabs 前，建議先移除此濾鏡；更新後只安裝明確標示支援該版本的 Release。

若 Streamlabs 無法啟動，可在 Windows「已安裝的應用程式」移除 **Pure Gaussian Blur for Streamlabs**，或刪除其外掛 DLL 與資料目錄。詳細安全回報方式請見 [SECURITY.md](SECURITY.md)。

## 檔案驗證

`MotionFrostedGlass-Setup-0.1.6.exe`

```text
SHA-256  78107D61E82461FD79353DD307A8A8921B4C702B9C50B65F5DF6D1E7416BCE70
```

PowerShell 驗證方式：

```powershell
Get-FileHash .\MotionFrostedGlass-Setup-0.1.6.exe -Algorithm SHA256
```

## 從原始碼建置

需要 Visual Studio 2022、CMake、Windows SDK、Inno Setup 6，以及 Streamlabs Desktop 1.21.4 的相容 OBS runtime。ABI 標頭對應 Streamlabs `obs-studio` fork 的 `31.1.2ndi1` 標籤（commit `32985a8f9684035bfcfbea781046c88dba617024`）。

本儲存庫不散布 Streamlabs 的 `obs.dll` 或其他專有二進位檔。`compat/libobs` 只包含建置所需的相容標頭；完整來源、授權與出處請見 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。

## 授權與商標

本專案採用 **GPL-2.0-or-later**，詳見 [LICENSE](LICENSE)。Streamlabs 與 OBS 名稱及商標屬各自權利人所有。

## English summary

An unofficial Windows x64 privacy-grade Gaussian blur filter for **Streamlabs Desktop 1.21.4** with embedded OBS runtime 31.1.2. It supports full-source or percentage-based rectangular regions, adjustable strength and three quality levels. Download the verified installer from [GitHub Releases](../../releases). Do not install it alongside another plugin using the `shader_filter` source ID. Streamlabs updates may remove or break the plugin.
