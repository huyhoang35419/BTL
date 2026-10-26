# BÀI TẬP LỚN: HỆ THỐNG QUẢN LÝ KHO HÀNG & BÁN HÀNG (WMS)
> **Môn học:** Lập trình hướng đối tượng (OOP)  
> **Ngôn ngữ:** C++ (C++20 thuần)  
> **Kiến trúc:** Web Server nhúng Winsock C++ kết hợp Frontend HTML/CSS/JS  
> **Tone màu giao diện:** Xanh nhạt & Trắng (Light Blue & White Theme)  
> **Cơ sở dữ liệu:** Nhúng CSDL quan hệ trực tiếp trong C++ (`Database.h` / `CoSoDuLieu.h`), tự động nạp từ `database/wms.sql` và lưu bền vững vào `database/wms.db`.

---

## I. TỔNG QUAN HỆ THỐNG & ĐÁP ỨNG NGHIỆP VỤ

Hệ thống được xây dựng hoàn chỉnh theo yêu cầu file tài liệu nghiệp vụ **"Nghiệp vụ BTL OOP"**:
1. **Phân quyền 4 vai trò rõ rệt:**
   - **Admin (Quản trị viên):** Quản lý tài khoản người dùng, phân quyền hệ thống, xem báo cáo tổng hợp doanh thu, tồn kho và đơn hàng.
   - **Người quản lý kho (Thủ kho):** Lập phiếu nhập kho từ nhà cung cấp (PO), quản lý vị trí khu/kệ hàng, kiểm kê đối chiếu thực tế và sổ sách, nhận cảnh báo hết hàng/cận hạn dùng.
   - **Người bán hàng (Nhân viên đơn online):** Tiếp nhận đơn online từ website, duyệt/từ chối đơn hàng, xuất giao, thu tiền và hủy đơn hàng.
   - **Khách hàng:** Đăng ký tài khoản, xác thực OTP qua email, xem hàng, chọn mua, áp dụng khuyến mãi & thuế, đặt hàng trực tuyến và theo dõi lịch sử đơn hàng.

2. **Giao diện hiện đại - Tone màu Xanh nhạt & Trắng:**
   - Màu sắc chủ đạo: Xanh da trời nhạt dịu mát (`#e0f2fe`, `#0284c7`, `#38bdf8`) kết hợp màu nền trắng ngọc (`#ffffff`, `#f0f7ff`).
   - Bố cục responsive, thân thiện với người dùng, hiển thị thông số trực quan.

---

## II. CÁC NGUYÊN LÝ LẬP TRÌNH HƯỚNG ĐỐI TƯỢNG (OOP)

### 1. Kế Thừa & Đa Hình (Inheritance & Polymorphism)
- **Lớp cơ sở trừu tượng:** `SanPham` (`SanPham.h`) chứa các thuộc tính cốt lõi và phương thức thuần ảo:
  - `virtual LoaiSanPham layLoai() const = 0;`
  - `virtual double tinhGiaBanThucTe(double soLuong) const = 0;`
  - `virtual bool kiemTraXuatKho(double soLuong, string& lyDo) const = 0;`
  - `virtual string xuatThongTinJSON() const = 0;`
- **3 Lớp con dẫn xuất đa hình:**
  1. `SanPhamThuong`: Dành cho hàng gia dụng, văn phòng phẩm tiêu chuẩn.
  2. `SanPhamHanSuDung`: Quản lý ngày sản xuất, hạn sử dụng, tính số ngày còn lại, áp dụng nguyên tắc **FEFO** (Hết hạn trước xuất trước) và tự động giảm giá 30% khi cận date.
  3. `SanPhamTheoCan`: Quản lý hàng bán theo cân (Kg/gam), tính tỷ lệ hao hụt tự nhiên khi xuất kho (`heSoHaoHut`).

- **Kế thừa Người dùng:** Lớp cơ sở `NguoiDung` dẫn xuất thành `QuanTriVien`, `NguoiQuanLyKho`, `NhanVienBanHang`, và `KhachHang`.

### 2. Các Mẫu Thiết Kế (Design Patterns)
- **Strategy Pattern (`Strategy.h`):**
  - *Chiến lược Khuyến mãi / Giảm giá:* `IChienLuocGiamGia` với các lớp cụ thể:
    + `KhongGiamGia`: Không chiết khấu.
    + `GiamGiaPhanTram`: Giảm 10% tổng đơn hàng.
    + `GiamGiaSoTienCoDinh`: Giảm 30.000₫ cho đơn từ 200.000₫.
    + `GiamGiaKhachHangVIP`: Tri ân khách VIP giảm 15%.
  - *Chiến lược Tính thuế:* `IChienLuocThue` (`ThueVAT10`, `ThueVAT5`, `ThueMienGiam`).
- **Observer Pattern (`Observer.h`):**
  - `TrungTamCanhBaoKho` (Subject) liên tục theo dõi tình trạng kho.
  - Khi hàng tồn $\le$ mức tối thiểu (Min) hoặc sản phẩm có date sắp hết hạn trong 7 ngày, hệ thống tự động bắn sự kiện cảnh báo đến `QuanSatThuKho` và `QuanSatBanHang` (Observers) và đẩy banner lên giao diện web.
- **Command Pattern (`Command.h`):**
  - Đóng gói thao tác hủy đơn thành `LenhHuyHoaDon` kế thừa interface `ILenh` (`thucHien()`, `hoanTac()`).
  - **Tránh lỗi nghiệp vụ cộng tồn kho 2 lần:** Lệnh hủy kiểm tra chính xác đơn đã xuất kho hay chưa, ghi nhận trạng thái đã hoàn kho, và cho phép hoàn tác (Undo) phục hồi lại trạng thái cũ chuẩn xác.

---

## III. CẤU TRÚC THƯ MỤC CHUẨN

```
WMS/
├── backend/
│   ├── main.cpp              # Điểm khởi chạy chương trình & nạp cấu hình .env
│   ├── MayChuWeb.h / .cpp    # Web Server HTTP đa luồng Winsock C++
│   ├── CoSoDuLieu.h / .cpp   # CSDL nhúng, CRUD, lưu bền vững vào đĩa wms.db
│   ├── Database.h / .cpp     # Header alias Database theo chuẩn tài liệu
│   ├── SanPham.h / .cpp      # Kế thừa & Đa hình (Thường, HSD, Theo cân)
│   ├── NguoiDung.h / .cpp    # Kế thừa Người dùng (Admin, Kho, Bán hàng, Khách)
│   ├── HoaDon.h / .cpp       # Nghiệp vụ hóa đơn, áp dụng Strategy
│   ├── QuanLyKho.h / .cpp    # Nhập kho PO, kiểm kê chênh lệch
│   ├── QuanLyBanHang.h / .cpp # Xử lý đơn online, chống bán vượt tồn
│   ├── Strategy.h            # Design Pattern: Strategy (Khuyến mãi & Thuế)
│   ├── Observer.h            # Design Pattern: Observer (Cảnh báo tồn kho & HSD)
│   ├── Command.h / .cpp      # Design Pattern: Command (Hủy đơn & Hoàn tác)
│   ├── AuthService.h / .cpp  # Đăng nhập, đăng ký, xác thực
│   ├── OTPService.h / .cpp   # Tạo và xác thực OTP 6 số (thời hạn 5 phút)
│   └── EmailService.h / .cpp # Mô phỏng gửi email thông báo
│
├── frontend/
│   ├── index.html            # Trang chủ & Cửa hàng mua sắm online
│   ├── login.html            # Đăng nhập phân quyền
│   ├── register.html         # Đăng ký tài khoản mới
│   ├── verify-otp.html       # Nhập mã OTP kích hoạt tài khoản
│   ├── forgot-password.html  # Quên mật khẩu
│   ├── reset-password.html   # Đặt lại mật khẩu mới qua OTP
│   ├── sanpham.html          # Quản lý danh mục hàng hóa (OOP Đa hình)
│   ├── kho.html              # Quản lý kho: Nhập kho PO, Kiểm kê, Cảnh báo Observer
│   ├── banhang.html          # Bán hàng online, duyệt đơn, xuất hàng
│   ├── hoadon.html           # Xem & In hóa đơn VAT, Command Pattern Hủy/Hoàn tác
│   ├── baocao.html           # Thống kê doanh thu, tồn kho & Quản lý User
│   ├── css/
│   │   └── style.css         # Bộ CSS tông màu Xanh nhạt & Trắng thanh lịch
│   └── js/
│       ├── app.js            # Kết nối API backend C++, giỏ hàng, thông báo
│       └── auth.js           # Quản lý token, phân quyền giao diện
│
├── database/
│   └── wms.sql               # Kịch bản SQL tạo bảng và nạp dữ liệu khởi tạo
│
├── config/
│   └── .env                  # Cấu hình cổng mạng PORT=8080, CSDL
│
├── build.bat                 # Script biên dịch 1-click bằng g++
├── run.bat                   # Script khởi chạy máy chủ & tự động mở trình duyệt
└── README.md
```

---

## IV. TÀI KHOẢN THỬ NGHIỆM ĐÃ TẠO SẴN

| Vai trò | Tên đăng nhập | Mật khẩu | Chức năng chính |
| :--- | :--- | :--- | :--- |
| **Quản trị viên (Admin)** | `admin` | `admin123` | Quản lý tài khoản, phân quyền, xem báo cáo doanh thu & tồn kho |
| **Quản lý kho (Thủ kho)** | `thukho` | `kho123` | Lập phiếu nhập kho, kiểm kê chênh lệch, nhận cảnh báo Observer |
| **Nhân viên bán hàng** | `banhang` | `banhang123` | Duyệt đơn online, xuất giao hàng, xác nhận thu tiền |
| **Khách hàng** | `khachhang` | `khach123` | Mua hàng trực tuyến, áp dụng khuyến mãi & thuế, xem hóa đơn |

> *(Trang đăng nhập `login.html` có sẵn nút bấm 1-click để tự động điền tài khoản thử nghiệm nhanh chóng).*

---

## V. HƯỚNG DẪN BIÊN DỊCH VÀ CHẠY

### Cách 1: Chạy trực tiếp (Nhanh nhất)
1. Nhấp đúp chuột vào file **`run.bat`** tại thư mục gốc hoặc trong thư mục `WMS/`.
2. Trình duyệt web sẽ tự động mở trang chủ tại địa chỉ: **`http://localhost:8080`**.

### Cách 2: Tự biên dịch lại bằng Command Line
Mở PowerShell hoặc Command Prompt tại thư mục dự án và chạy:
```powershell
.\build.bat
.\run.bat
```
Hoặc lệnh biên dịch trực tiếp bằng `g++`:
```powershell
cd WMS/backend
g++ -std=c++20 -Wall -Wextra -O2 main.cpp CoSoDuLieu.cpp SanPham.cpp HoaDon.cpp QuanLyKho.cpp QuanLyBanHang.cpp NguoiDung.cpp AuthService.cpp EmailService.cpp OTPService.cpp MayChuWeb.cpp -lws2_32 -o wms_server.exe
.\wms_server.exe
```

---

## VI. CÁC CÂU HỎI THƯỜNG GẶP KHI BẢO VỆ BÀI TẬP LỚN

1. **Làm thế nào để tránh bán hàng sai tồn kho (Overselling)?**
   - Trước khi lưu hóa đơn, backend duyệt danh sách giỏ hàng và gọi phương thức `sp->kiemTraXuatKho(soLuong)`. Nếu bất kỳ món nào không đủ tồn kho hoặc đã hết hạn sử dụng, giao dịch sẽ bị từ chối ngay lập tức và trả thông báo lỗi cụ thể cho khách hàng.

2. **Cách tính tiền đảm bảo đúng giá tại thời điểm đặt?**
   - Trong `QuanLyBanHang.cpp`, đơn giá của mỗi dòng chi tiết `ChiTietHoaDon` được sao chép trực tiếp từ `sp->tinhGiaBanThucTe()` tại thời điểm tạo đơn. Dù sau này giá sản phẩm trong danh mục có tăng hay giảm thì hóa đơn đã lập vẫn bảo lưu đúng giá trị gốc.

3. **Làm sao để hủy đơn không bị cộng trùng tồn kho hai lần?**
   - Thông qua **Command Pattern** (`LenhHuyHoaDon`): Mỗi thao tác hủy đều kiểm tra trạng thái đơn hàng (`CHO_XAC_NHAN`, `DA_XAC_NHAN`, `DANG_GIAO`). Nếu đơn đã xuất hàng thì lệnh mới hoàn trả tồn kho và lưu biến trạng thái `daTungHoanKho = true`. Khi người dùng thực hiện hoàn tác (Undo), hệ thống sẽ trừ ngược lại tồn kho một cách an toàn.

