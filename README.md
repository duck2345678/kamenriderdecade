# 🎮 Kamen Rider Decade: Dimensional Convergence
> **Đồ án môn học:** SE102 - Nhập môn phát triển game (Introduction to Game Programming)  
> **Trường:** Đại học Công nghệ Thông tin - ĐHQG TP.HCM (UIT)  
> **Khoa:** Công nghệ Phần mềm  
> **Giảng viên hướng dẫn:** ThS. Đinh Nguyễn Anh Dũng  

---

## 📖 1. Giới thiệu dự án
**Kamen Rider Decade: Dimensional Convergence** là một tựa game 2D Action Platformer thời gian thực được xây dựng bằng **C++ và DirectX 9/11 SDK (Direct3D, DirectInput, DirectSound)**.

Trò chơi lấy cảm hứng từ series Tokusatsu huyền thoại *Kamen Rider Decade*. Người chơi sẽ điều khiển Tsukasa Kadoya (Decade) vượt qua các vết nứt không gian nơi 9 thế giới Heisei tiền nhiệm đang bị dung hợp lại để đối đầu với các thế lực đen tối và bảo vệ sự tồn vong của đa vũ trụ.

---

## 🌟 2. Các tính năng & Cơ chế nổi bật (Game Features)
1. **Hệ thống Rider Cards (Decadriver):**
   - **Ride Booker (Sword & Gun):** Chuyển đổi linh hoạt giữa đòn đánh cận chiến chém combo và bắn súng tầm xa.
   - **Attack Ride: Clock Up (Thế giới Kabuto):** Can thiệp trực tiếp vào Game Loop và Delta Time ($dt$) để làm chậm thời gian toàn bộ thế giới xung quanh.
   - **Attack Ride: Blast:** Bắn chùm đạn laser quét sạch quái bay.
   - **Final Attack Ride: Dimension Kick:** Chiêu tất sát lướt qua hàng loạt thẻ bài ánh sáng để tung cú đá kết liễu Boss.
2. **Thiết kế màn chơi liền mạch (Seamless Multi-Zone Map):**
   - 1 Map lớn duy nhất được phân chia thành 5 phân vùng thế giới:
     - **Zone 1: World of Kuuga** (Đền thờ cổ Gurongi - Cơ bản)
     - **Zone 2: World of Faiz** (Smart Brain Lab - Bệ di chuyển, laser, dốc nghiêng)
     - **Zone 3: World of Kabuto** (Nhà máy chông xoay tốc độ cao - Clock Up)
     - **Zone 4: World of Den-O** (Nóc tàu thời gian DenLiner - Gió thổi ngược chiều)
     - **Zone 5: The Collapse / Boss Room** (Thế giới Hủy Diệt - Camera Lock & Trùm cuối)
3. **Kỹ thuật nền tảng tối ưu (Engine Core):**
   - **Xử lý va chạm:** Thuật toán Swept AABB hoàn chỉnh (xử lý va chạm động với tĩnh, động với động, bệ một chiều, dốc nghiêng).
   - **Phân chia không gian:** QuadTree tối ưu hóa hiệu năng, duy trì ổn định 60 FPS.
   - **Hệ thống cảnh:** Scene Manager quản lý IntroScene, PlayScene, GameOverScene.
   - **Âm thanh:** DirectSound hỗ trợ nhạc nền BGM loop và hiệu ứng âm thanh SFX thắt lưng Decadriver sống động.

---

## 📁 3. Cấu trúc thư mục mã nguồn (Source Tree)
```
kamenriderdecade/
├── Assets/                 # Tài nguyên game (Textures, Audio, Tilemaps)
│   ├── Audio/              # Nhạc nền .wav, SFX giọng đọc thắt lưng
│   ├── Maps/               # File Tiled Map (.tmx, .json) & Tilesets
│   └── Textures/           # Spritesheets của Decade, Quái, Boss, UI
│
├── Framework/              # Bộ khung Game Engine DirectX cơ sở
│   ├── Constants.h         # Các thông số cấu hình màn hình, FPS, tag
│   ├── Game.h / .cpp       # Vòng lặp game, Device Direct3D, Window Proc
│   ├── Camera.h / .cpp     # Camera 2D theo dõi nhân vật, Camera Lock
│   ├── DirectInput.h / .cpp# Xử lý bàn phím & chuột
│   ├── SoundManager.h / .cpp# DirectSound nạp và phát âm thanh
│   ├── TextureManager.h    # Quản lý & cache texture
│   └── Sprite.h / .cpp     # Cắt khung hình spritesheet, quản lý animation
│
├── Physics/                # Toán & Xử lý va chạm
│   ├── SweptAABB.h / .cpp  # Thuật toán dự đoán va chạm hộp giới hạn
│   ├── QuadTree.h / .cpp   # Cây tứ phân quản lý đối tượng trong không gian
│   └── TileMap.h / .cpp    # Parser nạp dữ liệu gạch và lớp va chạm
│
├── Objects/                # Phân cấp hướng đối tượng (OOP)
│   ├── GameObject.h / .cpp # Lớp cơ sở (Position, Velocity, Hitbox, State)
│   ├── Player/             # Logic nhân vật chính Decade
│   ├── Enemies/            # AI quái vật và Boss
│   ├── Weapons/            # Đạn súng, nhát chém kiếm, hiệu ứng thẻ bài
│   └── Environment/        # Bệ cứng, bệ di chuyển, bẫy chông, cổng cực quang
│
├── Scenes/                 # Quản lý màn chơi
│   ├── Scene.h             # Lớp Scene trừu tượng
│   ├── SceneManager.h      # Điều phối chuyển cảnh
│   └── PlayScene.h / .cpp  # Màn chơi chính 5 phân vùng
│
└── main.cpp                # WinMain entry point của ứng dụng
```

---

## 🛠️ 4. Hướng dẫn cài đặt & Chạy dự án
1. **Yêu cầu môi trường:**
   - Hệ điều hành: Windows 10 / 11
   - IDE: Microsoft Visual Studio (2019 / 2022) với workload *Desktop development with C++*.
   - DirectX SDK: Microsoft DirectX SDK (June 2010) hoặc Windows SDK tương thích.
2. **Cấu hình Project trong Visual Studio:**
   - Include Directories: Thêm đường dẫn `$(DXSDK_DIR)Include;`
   - Library Directories: Thêm đường dẫn `$(DXSDK_DIR)Lib\x86;`
   - Additional Dependencies: `d3d9.lib`, `d3dx9.lib`, `dinput8.lib`, `dxguid.lib`, `dsound.lib`, `winmm.lib`
3. **Build & Chạy:**
   - Chọn cấu hình `Debug` hoặc `Release` trên nền tảng `x86 (Win32)`.
   - Nhấn `F5` để biên dịch và chạy game.

---

## 👥 5. Thành viên nhóm & Phân công
- Sinh viên 1: ...
- Sinh viên 2: ...
- Sinh viên 3: ...
