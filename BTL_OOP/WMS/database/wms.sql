-- ========================================================
-- CO SO DU LIEU: QUAN LY KHO HANG VA BAN HANG (WMS)
-- Mon hoc: Lap trinh huong doi tuong (OOP)
-- ========================================================

-- 1. Bang NguoiDung (Tai khoan nguoi dung he thong)
CREATE TABLE IF NOT EXISTS NguoiDung (
    maNguoiDung VARCHAR(20) PRIMARY KEY,
    tenDangNhap VARCHAR(50) UNIQUE NOT NULL,
    matKhau VARCHAR(100) NOT NULL,
    hoTen NVARCHAR(100) NOT NULL,
    email VARCHAR(100) UNIQUE NOT NULL,
    soDienThoai VARCHAR(20),
    vaiTro VARCHAR(30) NOT NULL, -- ADMIN, QUAN_LY_KHO, BAN_HANG, KHACH_HANG
    trangThai VARCHAR(20) DEFAULT 'KICH_HOAT',
    ngayTao DATETIME DEFAULT CURRENT_TIMESTAMP
);

-- 2. Bang SanPham (Hang hoa trong kho & ban hang)
CREATE TABLE IF NOT EXISTS SanPham (
    maSP VARCHAR(20) PRIMARY KEY,
    tenSP NVARCHAR(200) NOT NULL,
    loaiSP VARCHAR(30) NOT NULL, -- THUONG, HAN_SU_DUNG, THEO_CAN
    donViTinh NVARCHAR(20) NOT NULL, -- Cai, Hop, Kg, Gam, Chai
    giaNhap DOUBLE NOT NULL,
    giaBan DOUBLE NOT NULL,
    soLuongTon DOUBLE DEFAULT 0,
    tonToiThieu DOUBLE DEFAULT 5,
    tonToiDa DOUBLE DEFAULT 500,
    viTriKho VARCHAR(50) DEFAULT 'Khu A - Ke 01',
    ngaySanXuat VARCHAR(20) DEFAULT '',
    hanSuDung VARCHAR(20) DEFAULT '',
    trongLuong DOUBLE DEFAULT 1.0,
    heSoHaoHut DOUBLE DEFAULT 0.0,
    moTa NVARCHAR(500) DEFAULT '',
    hinhAnh VARCHAR(255) DEFAULT ''
);

-- 3. Bang PhieuNhapKho (Nghiep vu nhap hang)
CREATE TABLE IF NOT EXISTS PhieuNhapKho (
    maPhieu VARCHAR(30) PRIMARY KEY,
    ngayNhap DATETIME DEFAULT CURRENT_TIMESTAMP,
    nhaCungCap NVARCHAR(150) NOT NULL,
    nguoiLapPhieu VARCHAR(50) NOT NULL,
    tongTien DOUBLE DEFAULT 0,
    ghiChu NVARCHAR(300) DEFAULT ''
);

-- 4. Bang ChiTietNhapKho
CREATE TABLE IF NOT EXISTS ChiTietNhapKho (
    maChiTiet VARCHAR(30) PRIMARY KEY,
    maPhieu VARCHAR(30) NOT NULL,
    maSP VARCHAR(20) NOT NULL,
    soLuong DOUBLE NOT NULL,
    donGiaNhap DOUBLE NOT NULL,
    hanSuDung VARCHAR(20) DEFAULT '',
    FOREIGN KEY (maPhieu) REFERENCES PhieuNhapKho(maPhieu),
    FOREIGN KEY (maSP) REFERENCES SanPham(maSP)
);

-- 5. Bang HoaDon (Nghiep vu ban hang & don online)
CREATE TABLE IF NOT EXISTS HoaDon (
    maHoaDon VARCHAR(30) PRIMARY KEY,
    ngayTao DATETIME DEFAULT CURRENT_TIMESTAMP,
    maKhachHang VARCHAR(50),
    tenKhachHang NVARCHAR(100) NOT NULL,
    soDienThoai VARCHAR(20),
    diaChiGiaoHang NVARCHAR(250),
    tongTienGoc DOUBLE DEFAULT 0,
    loaiGiamGia VARCHAR(50) DEFAULT 'KHONG_GIAM',
    tienGiamGia DOUBLE DEFAULT 0,
    loaiThue VARCHAR(30) DEFAULT 'VAT_10',
    tienThue DOUBLE DEFAULT 0,
    tongTienThanhToan DOUBLE DEFAULT 0,
    phuongThucThanhToan VARCHAR(50) DEFAULT 'TIEN_MAT', -- TIEN_MAT, CHUYEN_KHOAN
    trangThaiDon VARCHAR(30) DEFAULT 'CHO_XAC_NHAN', -- CHO_XAC_NHAN, DA_XAC_NHAN, DANG_GIAO, DA_GIAO, DA_HUY, DA_TRA_HANG
    trangThaiThanhToan VARCHAR(30) DEFAULT 'CHUA_THANH_TOAN', -- CHUA_THANH_TOAN, DA_THANH_TOAN, DA_HOAN_TIEN
    nguoiXuLy VARCHAR(50) DEFAULT 'System',
    ghiChu NVARCHAR(300) DEFAULT ''
);

-- 6. Bang ChiTietHoaDon
CREATE TABLE IF NOT EXISTS ChiTietHoaDon (
    maChiTiet VARCHAR(30) PRIMARY KEY,
    maHoaDon VARCHAR(30) NOT NULL,
    maSP VARCHAR(20) NOT NULL,
    tenSP NVARCHAR(200) NOT NULL,
    soLuong DOUBLE NOT NULL,
    donGia DOUBLE NOT NULL,
    thanhTien DOUBLE NOT NULL,
    FOREIGN KEY (maHoaDon) REFERENCES HoaDon(maHoaDon),
    FOREIGN KEY (maSP) REFERENCES SanPham(maSP)
);

-- 7. Bang PhieuKiemKe (Doi chieu so lieu thuc te va so sach)
CREATE TABLE IF NOT EXISTS PhieuKiemKe (
    maKiemKe VARCHAR(30) PRIMARY KEY,
    ngayKiemKe DATETIME DEFAULT CURRENT_TIMESTAMP,
    nguoiKiemKe VARCHAR(50) NOT NULL,
    maSP VARCHAR(20) NOT NULL,
    tonSoSach DOUBLE NOT NULL,
    tonThucTe DOUBLE NOT NULL,
    chenhLech DOUBLE NOT NULL,
    lyDo NVARCHAR(200) DEFAULT '',
    cachXuLy NVARCHAR(200) DEFAULT ''
);

-- 8. Bang CanhBao (Observer thong bao het hang, can date)
CREATE TABLE IF NOT EXISTS CanhBao (
    maCanhBao VARCHAR(30) PRIMARY KEY,
    thoiGian DATETIME DEFAULT CURRENT_TIMESTAMP,
    loaiCanhBao VARCHAR(50) NOT NULL, -- HET_HANG, SAP_HET_HAN, CHENH_LECH_KHO
    maSP VARCHAR(20),
    noiDung NVARCHAR(300) NOT NULL,
    daDoc INT DEFAULT 0
);

-- ========================================================
-- DU LIEU KHOI TAO SAN (SEEDED DATA)
-- ========================================================

-- Nguoi dung mau:
INSERT INTO NguoiDung VALUES 
('ND01', 'admin', 'admin123', N'Quản Trị Viên Hệ Thống', 'admin@wms.com', '0901234567', 'ADMIN', 'KICH_HOAT', CURRENT_TIMESTAMP),
('ND02', 'thukho', 'kho123', N'Nguyễn Văn Kho (Thủ Kho)', 'thukho@wms.com', '0912345678', 'QUAN_LY_KHO', 'KICH_HOAT', CURRENT_TIMESTAMP),
('ND03', 'banhang', 'banhang123', N'Trần Thị Bán Hàng', 'banhang@wms.com', '0923456789', 'BAN_HANG', 'KICH_HOAT', CURRENT_TIMESTAMP),
('ND04', 'khachhang', 'khach123', N'Lê Hoàng Khách Mua', 'khachhang@gmail.com', '0934567890', 'KHACH_HANG', 'KICH_HOAT', CURRENT_TIMESTAMP);

-- San pham mau (du ca 3 loai theo OOP da hinh):
-- 1. SanPhamThuong: Binh giu nhiet Lock&Lock, Ban phim co Logitech
-- 2. SanPhamHanSuDung: Sua tuoi Vinamilk 1L, Banh mi sandwich Kinh Do
-- 3. SanPhamTheoCan: Gao ST25 thom ngon, Thit bo My tuoi ngon
INSERT INTO SanPham VALUES
('SP01', N'Bình giữ nhiệt Inox 800ml', 'THUONG', N'Cái', 120000, 185000, 45, 10, 200, 'Khu A - Ke 01', '', '', 1.0, 0.0, N'Bình giữ nhiệt chân không cao cấp', 'binh-inox.jpg'),
('SP02', N'Bàn phím cơ không dây E-Dra', 'THUONG', N'Chiếc', 450000, 690000, 18, 5, 100, 'Khu A - Ke 02', '', '', 1.0, 0.0, N'Switch Brown, kết nối Bluetooth', 'ban-phim.jpg'),
('SP03', N'Sữa tươi thanh trùng Vinamilk 1L', 'HAN_SU_DUNG', N'Hộp', 28000, 39000, 60, 15, 300, 'Khu B (Lạnh) - Kệ 01', '2026-10-15', '2026-11-05', 1.0, 0.0, N'Bảo quản mát 4-6 độ C', 'sua-tuoi.jpg'),
('SP04', N'Bánh mì sandwich bơ Pháp', 'HAN_SU_DUNG', N'Túi', 15000, 24000, 4, 10, 100, 'Khu B - Ke 02', '2026-10-20', '2026-10-28', 1.0, 0.0, N'Hàng tươi mới mỗi ngày - Sắp hết hạn', 'banh-mi.jpg'),
('SP05', N'Gạo thượng hạng ST25 túi 5kg', 'THEO_CAN', N'Kg', 32000, 42000, 120.5, 20, 500, 'Khu C - Ke 01', '', '', 5.0, 0.01, N'Gạo dẻo thơm thượng hạng thế giới', 'gao-st25.jpg'),
('SP06', N'Thịt ba chỉ bò Úc thái lát', 'THEO_CAN', N'Kg', 180000, 250000, 3.5, 10, 100, 'Khu B (Lạnh) - Kệ 03', '', '', 1.0, 0.02, N'Hàng đông lạnh nhập khẩu chính ngạch', 'thit-bo.jpg');

