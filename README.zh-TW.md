[English](README.md) | [繁體中文](README.zh-TW.md)

# Streamlabs 隱私高斯模糊濾鏡

這是一個非官方的 Windows 原生濾鏡，可對整個 Streamlabs Desktop 來源或以百分比指定的矩形區域套用隱私級高斯模糊，適合遮住文字、人物與其他敏感畫面資訊。

> [!IMPORTANT]
> 這是非官方社群專案，與 Streamlabs、OBS Project 無從屬或背書關係。它支援內建 **OBS Core API 31.1.x** 的 Streamlabs Desktop 版本。即使核心版本仍相容，Streamlabs 更新仍可能移除外掛或改變內部行為。

## 下載與安裝

1. 從 [GitHub Releases](https://github.com/fswin040/streamlabs-privacy-gaussian-filter/releases) 下載 `MotionFrostedGlass-Setup-0.1.6.exe`。
2. 完全關閉 Streamlabs Desktop。
3. 以系統管理員身分執行安裝程式。
4. 安裝器會檢查內建 `obs.dll` 的檔案版本，只有 OBS Core 31.1.x 才會繼續。
5. 在 Streamlabs 的來源上開啟「濾鏡」，加入 **純高斯模糊**。部分 Streamlabs 介面可能因相容性識別碼而顯示 **Shader**。

安裝檔尚未購買程式碼簽章，因此 Windows SmartScreen 可能顯示警告。請先核對下方 SHA-256，再決定是否選擇「其他資訊」→「仍要執行」。

## 功能

- 整個來源或自訂矩形模糊區域
- 以百分比調整區域位置與大小
- 模糊強度
- 低、中、高三種品質
- 一鍵重設預設值
- 強度為零時略過模糊運算
- 只處理含安全模糊邊距的指定區域，降低 GPU 工作量

此版本專注於真正的高斯模糊，沒有動畫、玻璃色調、彩色光效、邊框或顆粒效果。

## 相容性與安全性

安裝器與外掛有兩層獨立檢查：

- 安裝器讀取 Streamlabs 內建 `obs.dll` 的 Windows 檔案版本；只要 major 為 31、minor 為 1，就不限制 patch、build 或 Streamlabs Desktop 應用程式版本。
- 外掛在註冊濾鏡前再次檢查已載入的 OBS Core API，若不是 API 31.1.x 就拒絕載入。

已驗證環境：

- Windows x64
- Streamlabs Desktop 1.21.4
- 內建 `obs.dll` 檔案版本 31.1.2
- 執行時 OBS API 31.1.3

以上是已驗證範例，不是綁定 Streamlabs 應用程式版本。OBS Core 31.0.x、31.2.x、32.x、缺少 DLL 或無法讀取版本時都會拒絕安裝或載入。

Streamlabs 使用固定的濾鏡允許清單。本外掛使用 `shader_filter` 相容性來源識別碼，讓濾鏡能出現在介面中。請勿同時安裝其他使用相同識別碼的外掛，包括部分 OBS Shader Filter 版本。Streamlabs 更新可能刪除外掛檔案；只有更新後仍內建 OBS Core 31.1.x 時才應重新安裝。

## 移除或復原

完全關閉 Streamlabs Desktop，再從 Windows「已安裝的應用程式」移除 **Pure Gaussian Blur for Streamlabs**。

若 Streamlabs 無法啟動，請從其內部 `obs-studio-node` runtime 移除：

- `obs-plugins\64bit\motion-frosted-glass.dll`
- `data\obs-plugins\motion-frosted-glass`

安裝器不會取代 `obs.dll`、Streamlabs 執行檔或場景檔。安全回報方式請見 [SECURITY.md](SECURITY.md)。

## 驗證下載檔案

`MotionFrostedGlass-Setup-0.1.6.exe`

```text
SHA-256 E41AEC29133C1550DC49AAD8D677B131A6EA685B3519B0432E8E1B3F8F0CA2D3
```

PowerShell 驗證方式：

```powershell
Get-FileHash .\MotionFrostedGlass-Setup-0.1.6.exe -Algorithm SHA256
```

## 從原始碼建置

建置需要 Visual Studio 2022、CMake、Windows SDK、Inno Setup 6，以及相容的 Streamlabs OBS runtime。ABI 標頭對應 Streamlabs `obs-studio` fork 的 `31.1.2ndi1` 標籤、commit `32985a8f9684035bfcfbea781046c88dba617024`。

本儲存庫不散布 Streamlabs 的 `obs.dll` 或其他專有 runtime 二進位檔。`compat/libobs` 只包含相容性標頭；來源與聲明請見 [THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md)。

## 授權與商標

本專案採用 **GPL-2.0-or-later**，詳見 [LICENSE](LICENSE)。Streamlabs 與 OBS 名稱及商標屬各自權利人所有。
