# 安全性 / Security

## 支援範圍

目前只支援 Windows x64 的 Streamlabs Desktop 1.21.4 與內建 `obs.dll` 31.1.2。其他版本應視為不受支援，請勿強制安裝。

## 回報問題

若發現安全性問題，請不要公開貼出可利用的完整細節。請透過 GitHub 的 Private vulnerability reporting（若儲存庫頁面已啟用）回報；一般相容性或當機問題可建立 Issue，並附上：

- Streamlabs Desktop 完整版本
- `obs.dll` 檔案版本
- GPU 型號與驅動版本
- 重現步驟
- 移除個人資料後的 Streamlabs 日誌

切勿上傳串流金鑰、登入權杖或含私人資訊的完整場景設定。

## 緊急移除

完全關閉 Streamlabs Desktop，然後從 Windows「已安裝的應用程式」移除 **Pure Gaussian Blur for Streamlabs**。若仍無法啟動，再手動移除：

- `obs-studio-node\obs-plugins\64bit\motion-frosted-glass.dll`
- `obs-studio-node\data\obs-plugins\motion-frosted-glass`

## English

Only Streamlabs Desktop 1.21.4 on Windows x64 with embedded `obs.dll` 31.1.2 is supported. Report vulnerabilities privately when possible. Never attach stream keys, tokens, or unredacted scene data.
