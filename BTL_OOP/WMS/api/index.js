// ========================================================
// VERCEL SERVERLESS REST API: WMS BACKEND
// Tương thích 100% với C++ Winsock Web Server (MayChuWeb.cpp)
// ========================================================

// 1. DU LIEU KHOI TAO TRONG BO NHO (IN-MEMORY DATABASE SEED)
let db = {
    nguoiDung: [
        {
            maNguoiDung: 'ND01',
            tenDangNhap: 'admin',
            matKhau: 'admin123',
            hoTen: 'Quản Trị Viên Hệ Thống',
            email: 'admin@wms.com',
            soDienThoai: '0901234567',
            vaiTro: 'ADMIN',
            trangThai: 'KICH_HOAT'
        },
        {
            maNguoiDung: 'ND02',
            tenDangNhap: 'thukho',
            matKhau: 'kho123',
            hoTen: 'Nguyễn Văn Kho (Thủ Kho)',
            email: 'thukho@wms.com',
            soDienThoai: '0912345678',
            vaiTro: 'QUAN_LY_KHO',
            trangThai: 'KICH_HOAT'
        },
        {
            maNguoiDung: 'ND03',
            tenDangNhap: 'banhang',
            matKhau: 'banhang123',
            hoTen: 'Trần Thị Bán Hàng',
            email: 'banhang@wms.com',
            soDienThoai: '0923456789',
            vaiTro: 'BAN_HANG',
            trangThai: 'KICH_HOAT'
        },
        {
            maNguoiDung: 'ND04',
            tenDangNhap: 'khachhang',
            matKhau: 'khach123',
            hoTen: 'Lê Hoàng Khách Mua',
            email: 'khachhang@gmail.com',
            soDienThoai: '0934567890',
            vaiTro: 'KHACH_HANG',
            trangThai: 'KICH_HOAT'
        }
    ],

    sanPham: [
        {
            maSP: 'SP01',
            tenSP: 'Bình giữ nhiệt Inox 800ml',
            loaiSP: 'THUONG',
            donViTinh: 'Cái',
            giaNhap: 120000,
            giaBan: 185000,
            soLuongTon: 45,
            tonToiThieu: 10,
            tonToiDa: 200,
            viTriKho: 'Khu A - Ke 01',
            moTa: 'Bình giữ nhiệt chân không cao cấp',
            hinhAnh: 'binh-inox.jpg'
        },
        {
            maSP: 'SP02',
            tenSP: 'Bàn phím cơ không dây E-Dra',
            loaiSP: 'THUONG',
            donViTinh: 'Chiếc',
            giaNhap: 450000,
            giaBan: 690000,
            soLuongTon: 18,
            tonToiThieu: 5,
            tonToiDa: 100,
            viTriKho: 'Khu A - Ke 02',
            moTa: 'Switch Brown, kết nối Bluetooth',
            hinhAnh: 'ban-phim.jpg'
        },
        {
            maSP: 'SP03',
            tenSP: 'Sữa tươi thanh trùng Vinamilk 1L',
            loaiSP: 'HAN_SU_DUNG',
            donViTinh: 'Hộp',
            giaNhap: 28000,
            giaBan: 39000,
            soLuongTon: 60,
            tonToiThieu: 15,
            tonToiDa: 300,
            viTriKho: 'Khu B (Lạnh) - Kệ 01',
            moTa: 'Bảo quản mát 4-6 độ C',
            hinhAnh: 'sua-tuoi.jpg',
            ngaySanXuat: '2026-10-15',
            hanSuDung: '2026-11-20'
        },
        {
            maSP: 'SP04',
            tenSP: 'Bánh mì sandwich bơ Pháp',
            loaiSP: 'HAN_SU_DUNG',
            donViTinh: 'Túi',
            giaNhap: 15000,
            giaBan: 24000,
            soLuongTon: 4,
            tonToiThieu: 10,
            tonToiDa: 100,
            viTriKho: 'Khu B - Ke 02',
            moTa: 'Hàng tươi mới mỗi ngày - Sắp hết hạn',
            hinhAnh: 'banh-mi.jpg',
            ngaySanXuat: '2026-10-20',
            hanSuDung: '2026-10-28'
        },
        {
            maSP: 'SP05',
            tenSP: 'Gạo thượng hạng ST25 túi 5kg',
            loaiSP: 'THEO_CAN',
            donViTinh: 'Kg',
            giaNhap: 32000,
            giaBan: 42000,
            soLuongTon: 120.5,
            tonToiThieu: 20,
            tonToiDa: 500,
            viTriKho: 'Khu C - Ke 01',
            moTa: 'Gạo dẻo thơm thượng hạng thế giới',
            hinhAnh: 'gao-st25.jpg',
            trongLuong: 5.0,
            heSoHaoHut: 0.01
        },
        {
            maSP: 'SP06',
            tenSP: 'Thịt ba chỉ bò Úc thái lát',
            loaiSP: 'THEO_CAN',
            donViTinh: 'Kg',
            giaNhap: 180000,
            giaBan: 250000,
            soLuongTon: 3.5,
            tonToiThieu: 10,
            tonToiDa: 100,
            viTriKho: 'Khu B (Lạnh) - Kệ 03',
            moTa: 'Hàng đông lạnh nhập khẩu chính ngạch',
            hinhAnh: 'thit-bo.jpg',
            trongLuong: 1.0,
            heSoHaoHut: 0.02
        }
    ],

    phieuNhap: [],
    kiemKe: [],
    hoaDon: [],
    canhBao: [],
    otpMap: {}, // email -> otp
    tokens: {}, // token -> maNguoiDung
    commandHistory: [] // Command Pattern: Lich su huy de hoan tac
};

// Seed mot so du lieu don hang va canh bao mau
db.canhBao.push({
    maCanhBao: 'CB01',
    thoiGian: '2026-10-25 10:00:00',
    loaiCanhBao: 'HET_HANG',
    maSP: 'SP06',
    tenSP: 'Thịt ba chỉ bò Úc thái lát',
    noiDung: 'Sản phẩm Thịt ba chỉ bò Úc thái lát sắp hết hàng (Tồn: 3.5, Tối thiểu: 10)! Cần lập phiếu nhập kho.',
    daDoc: false
});

// Helper dinh dang thoi gian
function layThoiGianHienTai() {
    const d = new Date();
    const pad = (n) => String(n).padStart(2, '0');
    return `${d.getFullYear()}-${pad(d.getMonth() + 1)}-${pad(d.getDate())} ${pad(d.getHours())}:${pad(d.getMinutes())}:${pad(d.getSeconds())}`;
}

// Helper tinh so ngay con lai cua HSD (OOP SanPhamHanSuDung)
function tinhSoNgayConLai(hanSuDung) {
    if (!hanSuDung || hanSuDung.length < 10) return 999;
    try {
        const parts = hanSuDung.split('-');
        const target = new Date(parseInt(parts[0]), parseInt(parts[1]) - 1, parseInt(parts[2]));
        const now = new Date();
        now.setHours(0, 0, 0, 0);
        const diff = target.getTime() - now.getTime();
        return Math.ceil(diff / (1000 * 60 * 60 * 24));
    } catch (e) {
        return 999;
    }
}

// Format san pham xuat JSON day du thuoc tinh OOP
function formatSanPhamJSON(sp) {
    const item = { ...sp };
    item.canhBaoTon = item.soLuongTon <= item.tonToiThieu;

    if (item.loaiSP === 'HAN_SU_DUNG') {
        const ngayConLai = tinhSoNgayConLai(item.hanSuDung);
        item.ngayConLai = ngayConLai;
        item.daHetHan = ngayConLai < 0;
        item.sapHetHan = ngayConLai >= 0 && ngayConLai <= 7;
    }

    if (item.loaiSP === 'THEO_CAN') {
        item.trongLuong = item.trongLuong || 1.0;
        item.heSoHaoHut = item.heSoHaoHut || 0.0;
    }

    return item;
}

// Cap nhat canh bao tu dong (Observer Pattern)
function kiemTraVaPhatCanhBaoTuDong() {
    for (const sp of db.sanPham) {
        if (sp.soLuongTon <= sp.tonToiThieu) {
            const tonTai = db.canhBao.find(c => c.maSP === sp.maSP && c.loaiCanhBao === 'HET_HANG' && !c.daDoc);
            if (!tonTai) {
                db.canhBao.push({
                    maCanhBao: 'CB' + (db.canhBao.length + 1),
                    thoiGian: layThoiGianHienTai(),
                    loaiCanhBao: 'HET_HANG',
                    maSP: sp.maSP,
                    tenSP: sp.tenSP,
                    noiDung: `Sản phẩm ${sp.tenSP} sắp hết hàng (Tồn: ${sp.soLuongTon}, Tối thiểu: ${sp.tonToiThieu})! Cần lập phiếu nhập hàng.`,
                    daDoc: false
                });
            }
        }

        if (sp.loaiSP === 'HAN_SU_DUNG') {
            const ngay = tinhSoNgayConLai(sp.hanSuDung);
            if (ngay >= 0 && ngay <= 7) {
                const tonTai = db.canhBao.find(c => c.maSP === sp.maSP && c.loaiCanhBao === 'SAP_HET_HAN' && !c.daDoc);
                if (!tonTai) {
                    db.canhBao.push({
                        maCanhBao: 'CB' + (db.canhBao.length + 1),
                        thoiGian: layThoiGianHienTai(),
                        loaiCanhBao: 'SAP_HET_HAN',
                        maSP: sp.maSP,
                        tenSP: sp.tenSP,
                        noiDung: `Sản phẩm ${sp.tenSP} sắp hết hạn trong ${ngay} ngày (${sp.hanSuDung})! Cần ưu tiên xuất trước (FEFO).`,
                        daDoc: false
                    });
                }
            }
        }
    }
}

// Xac thuc Token tu Authorization header
function layNguoiDungTuToken(req) {
    const auth = req.headers['authorization'] || req.headers['Authorization'] || '';
    if (!auth || !auth.startsWith('Bearer ')) return null;
    const token = auth.substring(7);
    const maND = db.tokens[token];
    if (!maND) {
        // Fallback: neu token co dang TOKEN_<maND>_...
        const parts = token.split('_');
        if (parts.length >= 2) {
            return db.nguoiDung.find(u => u.maNguoiDung === parts[1]) || null;
        }
        return null;
    }
    return db.nguoiDung.find(u => u.maNguoiDung === maND) || null;
}

// ========================================================
// MAIN HANDLER VERCEL SERVERLESS FUNCTION
// ========================================================
module.exports = async (req, res) => {
    // 1. Thiet lap tieu de CORS
    res.setHeader('Access-Control-Allow-Origin', '*');
    res.setHeader('Access-Control-Allow-Methods', 'GET, POST, OPTIONS, PUT, DELETE');
    res.setHeader('Access-Control-Allow-Headers', 'Content-Type, Authorization');

    if (req.method === 'OPTIONS') {
        res.statusCode = 200;
        res.end('OK');
        return;
    }

    // Phan tich body JSON
    let body = req.body || {};
    if (typeof body === 'string' && body.trim().length > 0) {
        try {
            body = JSON.parse(body);
        } catch (e) {
            // body giu nguyen dang text hoac object rong
        }
    }

    // Phan tich duong dan URL
    const urlObj = new URL(req.url, `http://${req.headers.host || 'localhost'}`);
    let pathname = urlObj.pathname;

    // Chuan hoa pathname (bo qua /api neu can)
    if (!pathname.startsWith('/api')) {
        pathname = '/api' + pathname;
    }

    const query = Object.fromEntries(urlObj.searchParams);

    const json = (data, statusCode = 200) => {
        res.setHeader('Content-Type', 'application/json; charset=utf-8');
        res.statusCode = statusCode;
        res.end(JSON.stringify(data));
    };

    try {
        // ====================================================
        // AUTHENTICATION & USER MANAGEMENT
        // ====================================================
        if (pathname === '/api/dang-nhap' && req.method === 'POST') {
            const { tenDangNhap, matKhau } = body;
            const user = db.nguoiDung.find(u => u.tenDangNhap === tenDangNhap);
            if (!user) {
                return json({ thanhCong: false, thongBao: 'Tên đăng nhập không tồn tại trên hệ thống!' }, 400);
            }
            if (user.matKhau !== matKhau) {
                return json({ thanhCong: false, thongBao: 'Mật khẩu không chính xác!' }, 400);
            }
            if (user.trangThai === 'BI_KHOA') {
                return json({ thanhCong: false, thongBao: 'Tài khoản của bạn đang bị khóa!' }, 400);
            }

            const token = `TOKEN_${user.maNguoiDung}_${Date.now()}`;
            db.tokens[token] = user.maNguoiDung;

            return json({
                thanhCong: true,
                thongBao: 'Đăng nhập thành công!',
                token,
                nguoiDung: {
                    maNguoiDung: user.maNguoiDung,
                    tenDangNhap: user.tenDangNhap,
                    hoTen: user.hoTen,
                    email: user.email,
                    soDienThoai: user.soDienThoai,
                    vaiTro: user.vaiTro,
                    trangThai: user.trangThai
                }
            });
        }

        if (pathname === '/api/dang-ky' && req.method === 'POST') {
            const { tenDangNhap, matKhau, hoTen, email, soDienThoai } = body;
            if (db.nguoiDung.some(u => u.tenDangNhap === tenDangNhap)) {
                return json({ thanhCong: false, thongBao: 'Tên đăng nhập đã tồn tại!' }, 400);
            }
            if (db.nguoiDung.some(u => u.email === email)) {
                return json({ thanhCong: false, thongBao: 'Email đã được sử dụng!' }, 400);
            }

            const maNguoiDung = 'ND' + String(db.nguoiDung.length + 1).padStart(2, '0');
            const newUser = {
                maNguoiDung,
                tenDangNhap,
                matKhau,
                hoTen,
                email,
                soDienThoai: soDienThoai || '',
                vaiTro: 'KHACH_HANG',
                trangThai: 'CHUA_KICH_HOAT'
            };
            db.nguoiDung.push(newUser);

            // Sinh ma OTP kich hoat
            const otp = String(Math.floor(100000 + Math.random() * 900000));
            db.otpMap[email] = otp;

            return json({
                thanhCong: true,
                thongBao: 'Đăng ký thành công! Vui lòng nhập mã OTP để kích hoạt tài khoản.'
            });
        }

        if (pathname === '/api/xac-thuc-otp' && req.method === 'POST') {
            const { email, otp } = body;
            const otpLuu = db.otpMap[email];
            if (otpLuu && otpLuu === otp) {
                const user = db.nguoiDung.find(u => u.email === email);
                if (user) user.trangThai = 'KICH_HOAT';
                delete db.otpMap[email];
                return json({ thanhCong: true, thongBao: 'Kích hoạt tài khoản thành công! Bạn có thể đăng nhập.' });
            }
            return json({ thanhCong: false, thongBao: 'Mã OTP không chính xác hoặc đã hết hạn!' }, 400);
        }

        if (pathname === '/api/quen-mat-khau' && req.method === 'POST') {
            const { email } = body;
            const user = db.nguoiDung.find(u => u.email === email);
            if (!user) {
                return json({ thanhCong: false, thongBao: 'Không tìm thấy tài khoản với email này!' }, 400);
            }
            const otp = String(Math.floor(100000 + Math.random() * 900000));
            db.otpMap[email] = otp;
            return json({ thanhCong: true, thongBao: 'Mã xác thực đã được gửi về email của bạn.' });
        }

        if (pathname === '/api/dat-lai-mat-khau' && req.method === 'POST') {
            const { email, otp, matKhauMoi } = body;
            const otpLuu = db.otpMap[email];
            if (otpLuu && otpLuu === otp) {
                const user = db.nguoiDung.find(u => u.email === email);
                if (user) user.matKhau = matKhauMoi;
                delete db.otpMap[email];
                return json({ thanhCong: true, thongBao: 'Đặt lại mật khẩu thành công! Hãy đăng nhập bằng mật khẩu mới.' });
            }
            return json({ thanhCong: false, thongBao: 'Mã xác thực OTP không chính xác!' }, 400);
        }

        if (pathname === '/api/lay-otp-hien-tai' && req.method === 'GET') {
            const email = query.email || '';
            const otp = db.otpMap[email] || '123456';
            return json({ thanhCong: true, email, otp });
        }

        if (pathname === '/api/nguoi-dung' && req.method === 'GET') {
            return json({ thanhCong: true, danhSach: db.nguoiDung });
        }

        if (pathname === '/api/nguoi-dung/them' && req.method === 'POST') {
            const { tenDangNhap, matKhau, hoTen, email, soDienThoai, vaiTro } = body;
            if (db.nguoiDung.some(u => u.tenDangNhap === tenDangNhap || u.email === email)) {
                return json({ thanhCong: false, thongBao: 'Tên đăng nhập hoặc email đã tồn tại!' }, 400);
            }
            const maNguoiDung = 'ND' + String(db.nguoiDung.length + 1).padStart(2, '0');
            const nd = {
                maNguoiDung,
                tenDangNhap,
                matKhau,
                hoTen,
                email,
                soDienThoai: soDienThoai || '',
                vaiTro: vaiTro || 'KHACH_HANG',
                trangThai: 'KICH_HOAT'
            };
            db.nguoiDung.push(nd);
            return json({ thanhCong: true, thongBao: 'Thêm người dùng thành công!' });
        }

        if (pathname === '/api/nguoi-dung/sua' && req.method === 'POST') {
            const { maNguoiDung, hoTen, email, soDienThoai, matKhau } = body;
            const user = db.nguoiDung.find(u => u.maNguoiDung === maNguoiDung);
            if (!user) return json({ thanhCong: false, thongBao: 'Không tìm thấy người dùng!' }, 400);

            if (hoTen) user.hoTen = hoTen;
            if (email) user.email = email;
            if (soDienThoai) user.soDienThoai = soDienThoai;
            if (matKhau) user.matKhau = matKhau;
            return json({ thanhCong: true, thongBao: 'Cập nhật thành công!' });
        }

        if (pathname === '/api/nguoi-dung/xoa' && req.method === 'POST') {
            const { maNguoiDung } = body;
            const idx = db.nguoiDung.findIndex(u => u.maNguoiDung === maNguoiDung);
            if (idx === -1) return json({ thanhCong: false, thongBao: 'Không thể xóa tài khoản!' }, 400);
            db.nguoiDung.splice(idx, 1);
            return json({ thanhCong: true, thongBao: 'Xóa tài khoản thành công!' });
        }

        // ====================================================
        // SAN PHAM (OOP: THUONG, HAN_SU_DUNG, THEO_CAN)
        // ====================================================
        if (pathname === '/api/san-pham' && req.method === 'GET') {
            kiemTraVaPhatCanhBaoTuDong();
            const danhSach = db.sanPham.map(formatSanPhamJSON);
            return json({ thanhCong: true, danhSach });
        }

        if (pathname === '/api/san-pham/them' && req.method === 'POST') {
            let {
                maSP, tenSP, loaiSP, donViTinh, giaNhap, giaBan,
                soLuongTon, tonToiThieu, tonToiDa, viTriKho, moTa,
                ngaySanXuat, hanSuDung, trongLuong, heSoHaoHut
            } = body;

            if (!maSP) {
                maSP = 'SP' + String(db.sanPham.length + 1).padStart(2, '0');
            }
            if (db.sanPham.some(s => s.maSP === maSP)) {
                return json({ thanhCong: false, thongBao: 'Mã sản phẩm đã tồn tại!' }, 400);
            }

            const newSP = {
                maSP,
                tenSP: tenSP || 'Sản phẩm mới',
                loaiSP: loaiSP || 'THUONG',
                donViTinh: donViTinh || 'Cái',
                giaNhap: Number(giaNhap) || 0,
                giaBan: Number(giaBan) || 0,
                soLuongTon: Number(soLuongTon) || 0,
                tonToiThieu: Number(tonToiThieu) || 5,
                tonToiDa: Number(tonToiDa) || 500,
                viTriKho: viTriKho || 'Khu A - Ke 01',
                moTa: moTa || '',
                hinhAnh: body.hinhAnh || '',
                ngaySanXuat: ngaySanXuat || '',
                hanSuDung: hanSuDung || '',
                trongLuong: Number(trongLuong) || 1.0,
                heSoHaoHut: Number(heSoHaoHut) || 0.0
            };
            db.sanPham.push(newSP);
            kiemTraVaPhatCanhBaoTuDong();
            return json({ thanhCong: true, thongBao: 'Thêm sản phẩm thành công!' });
        }

        if (pathname === '/api/san-pham/sua' && req.method === 'POST') {
            const { maSP, tenSP, donViTinh, giaNhap, giaBan, tonToiThieu, tonToiDa, viTriKho, moTa, ngaySanXuat, hanSuDung } = body;
            const sp = db.sanPham.find(s => s.maSP === maSP);
            if (!sp) return json({ thanhCong: false, thongBao: 'Không tìm thấy sản phẩm!' }, 400);

            if (tenSP) sp.tenSP = tenSP;
            if (donViTinh) sp.donViTinh = donViTinh;
            if (giaNhap !== undefined) sp.giaNhap = Number(giaNhap);
            if (giaBan !== undefined) sp.giaBan = Number(giaBan);
            if (tonToiThieu !== undefined) sp.tonToiThieu = Number(tonToiThieu);
            if (tonToiDa !== undefined) sp.tonToiDa = Number(tonToiDa);
            if (viTriKho) sp.viTriKho = viTriKho;
            if (moTa !== undefined) sp.moTa = moTa;
            if (ngaySanXuat) sp.ngaySanXuat = ngaySanXuat;
            if (hanSuDung) sp.hanSuDung = hanSuDung;

            kiemTraVaPhatCanhBaoTuDong();
            return json({ thanhCong: true, thongBao: 'Cập nhật sản phẩm thành công!' });
        }

        if (pathname === '/api/san-pham/xoa' && req.method === 'POST') {
            const { maSP } = body;
            const idx = db.sanPham.findIndex(s => s.maSP === maSP);
            if (idx === -1) return json({ thanhCong: false, thongBao: 'Không thể xóa sản phẩm!' }, 400);
            db.sanPham.splice(idx, 1);
            return json({ thanhCong: true, thongBao: 'Xóa sản phẩm thành công!' });
        }

        // ====================================================
        // QUAN LY KHO: NHAP KHO (PO), KIEM KE, CANH BAO (OBSERVER)
        // ====================================================
        if (pathname === '/api/kho/nhap' && req.method === 'GET') {
            return json({ thanhCong: true, danhSach: db.phieuNhap });
        }

        if (pathname === '/api/kho/nhap' && req.method === 'POST') {
            const { nhaCungCap, nguoiLapPhieu, ghiChu, maSP, soLuong, donGiaNhap, hanSuDung } = body;
            const sp = db.sanPham.find(s => s.maSP === maSP);
            if (!sp) return json({ thanhCong: false, thongBao: 'Không tìm thấy sản phẩm cần nhập!' }, 400);

            const maPhieu = 'PN' + String(db.phieuNhap.length + 1).padStart(2, '0');
            const sl = Number(soLuong) || 1;
            const gia = Number(donGiaNhap) || sp.giaNhap;

            const phieu = {
                maPhieu,
                ngayNhap: layThoiGianHienTai(),
                nhaCungCap: nhaCungCap || 'Nhà cung cấp uy tín',
                nguoiLapPhieu: nguoiLapPhieu || 'Thủ kho',
                tongTien: sl * gia,
                ghiChu: ghiChu || '',
                trangThai: 'CHO_DUYET',
                danhSachChiTiet: [
                    {
                        maChiTiet: 'CTN_' + maPhieu,
                        maPhieu,
                        maSP,
                        tenSP: sp.tenSP,
                        soLuong: sl,
                        donGiaNhap: gia,
                        thanhTien: sl * gia,
                        hanSuDung: hanSuDung || ''
                    }
                ]
            };
            db.phieuNhap.push(phieu);
            return json({
                thanhCong: true,
                thongBao: 'Đã gửi phiếu nhập cho quản lý duyệt. Tồn kho sẽ cập nhật sau khi được phê duyệt.'
            });
        }

        if (pathname === '/api/kho/nhap/duyet' && req.method === 'POST') {
            const currentUser = layNguoiDungTuToken(req);
            if (!currentUser || currentUser.vaiTro !== 'ADMIN') {
                return json({ thanhCong: false, thongBao: 'Chỉ quản trị viên được phê duyệt phiếu nhập.' }, 403);
            }

            const { maPhieu } = body;
            const phieu = db.phieuNhap.find(p => p.maPhieu === maPhieu && p.trangThai === 'CHO_DUYET');
            if (!phieu) {
                return json({ thanhCong: false, thongBao: 'Không tìm thấy phiếu chờ duyệt hoặc phiếu đã được xử lý.' }, 400);
            }

            // Cap nhat ton kho cho tung mon
            for (const ct of phieu.danhSachChiTiet) {
                const sp = db.sanPham.find(s => s.maSP === ct.maSP);
                if (sp) {
                    sp.soLuongTon += ct.soLuong;
                    if (ct.hanSuDung && sp.loaiSP === 'HAN_SU_DUNG') {
                        sp.hanSuDung = ct.hanSuDung;
                    }
                }
            }
            phieu.trangThai = 'DA_DUYET';
            kiemTraVaPhatCanhBaoTuDong();

            return json({ thanhCong: true, thongBao: 'Đã duyệt phiếu nhập và cập nhật tồn kho.' });
        }

        if (pathname === '/api/kho/kiem-ke' && req.method === 'GET') {
            return json({ thanhCong: true, danhSach: db.kiemKe });
        }

        if (pathname === '/api/kho/kiem-ke' && req.method === 'POST') {
            const { maSP, tonThucTe, nguoiKiemKe, lyDo, cachXuLy } = body;
            const sp = db.sanPham.find(s => s.maSP === maSP);
            if (!sp) return json({ thanhCong: false, thongBao: 'Không tìm thấy sản phẩm!' }, 400);

            const tonSoSach = sp.soLuongTon;
            const thucTe = Number(tonThucTe) || 0;
            const chenhLech = thucTe - tonSoSach;

            const phieuKK = {
                maKiemKe: 'KK' + String(db.kiemKe.length + 1).padStart(2, '0'),
                ngayKiemKe: layThoiGianHienTai(),
                nguoiKiemKe: nguoiKiemKe || 'Kiểm kho viên',
                maSP,
                tenSP: sp.tenSP,
                tonSoSach,
                tonThucTe: thucTe,
                chenhLech,
                lyDo: lyDo || '',
                cachXuLy: cachXuLy || ''
            };
            db.kiemKe.push(phieuKK);
            // Can bang ton kho
            sp.soLuongTon = thucTe;
            kiemTraVaPhatCanhBaoTuDong();

            return json({ thanhCong: true, thongBao: 'Lập phiếu kiểm kê thành công! Đã cân bằng tồn kho.' });
        }

        if (pathname === '/api/kho/canh-bao' && req.method === 'GET') {
            kiemTraVaPhatCanhBaoTuDong();
            return json({ thanhCong: true, danhSach: db.canhBao });
        }

        if (pathname === '/api/kho/canh-bao/da-doc' && req.method === 'POST') {
            db.canhBao.forEach(c => c.daDoc = true);
            return json({ thanhCong: true });
        }

        // ====================================================
        // BAN HANG, HOA DON, STRATEGY PATTERN & COMMAND PATTERN
        // ====================================================
        if (pathname === '/api/hoa-don' && req.method === 'GET') {
            const currentUser = layNguoiDungTuToken(req);
            if (!currentUser) {
                return json({ thanhCong: false, thongBao: 'Vui lòng đăng nhập để xem đơn hàng.' }, 401);
            }

            let danhSach = db.hoaDon;
            if (currentUser.vaiTro === 'KHACH_HANG') {
                danhSach = db.hoaDon.filter(hd => hd.maKhachHang === currentUser.maNguoiDung);
            }

            return json({ thanhCong: true, danhSach });
        }

        if (pathname === '/api/don-hang/dat-hang' && req.method === 'POST') {
            const {
                maKhachHang, tenKhachHang, soDienThoai, diaChiGiaoHang,
                loaiGiamGia, loaiThue, phuongThucThanhToan, ghiChu, danhSachMon
            } = body;

            // Trich xuat danh sach mon hang: SP01:2,SP03:1
            const dsMon = [];
            if (typeof danhSachMon === 'string') {
                const items = danhSachMon.split(',');
                for (const it of items) {
                    const [ma, slStr] = it.split(':');
                    if (ma) {
                        dsMon.push({ maSP: ma.trim(), soLuong: Number(slStr) || 1 });
                    }
                }
            } else if (Array.isArray(danhSachMon)) {
                dsMon.push(...danhSachMon);
            }

            if (dsMon.length === 0) {
                return json({ thanhCong: false, thongBao: 'Giỏ hàng đang trống!' }, 400);
            }

            // Kiem tra ton kho va tinh tien
            let tongTienGoc = 0;
            const chiTietDon = [];

            for (const m of dsMon) {
                const sp = db.sanPham.find(s => s.maSP === m.maSP);
                if (!sp) {
                    return json({ thanhCong: false, thongBao: `Không tìm thấy sản phẩm ${m.maSP}!` }, 400);
                }
                if (sp.soLuongTon < m.soLuong) {
                    return json({
                        thanhCong: false,
                        thongBao: `Sản phẩm "${sp.tenSP}" không đủ số lượng trong kho (Còn: ${sp.soLuongTon})!`
                    }, 400);
                }

                // Kiem tra FEFO neu la san pham co HSD
                let donGiaBan = sp.giaBan;
                if (sp.loaiSP === 'HAN_SU_DUNG') {
                    const days = tinhSoNgayConLai(sp.hanSuDung);
                    if (days < 0) {
                        return json({
                            thanhCong: false,
                            thongBao: `Sản phẩm "${sp.tenSP}" đã hết hạn sử dụng! Không thể xuất bán.`
                        }, 400);
                    }
                    if (days <= 7) {
                        // Chinh sach tu dong giam gia 30% khi can date
                        donGiaBan = donGiaBan * 0.7;
                    }
                }

                const thanhTien = m.soLuong * donGiaBan;
                tongTienGoc += thanhTien;
                chiTietDon.push({
                    maChiTiet: 'CT_' + Date.now() + '_' + m.maSP,
                    maSP: sp.maSP,
                    tenSP: sp.tenSP,
                    soLuong: m.soLuong,
                    donGia: donGiaBan,
                    thanhTien
                });
            }

            // Strategy Pattern: Giam gia
            let tienGiamGia = 0;
            const maGG = loaiGiamGia || 'KHONG_GIAM';
            if (maGG === 'GIAM_PHAN_TRAM') {
                tienGiamGia = Math.min(tongTienGoc * 0.1, 500000);
            } else if (maGG === 'GIAM_SO_TIEN') {
                tienGiamGia = tongTienGoc >= 200000 ? 30000 : 0;
            } else if (maGG === 'GIAM_VIP') {
                tienGiamGia = tongTienGoc * 0.15;
            }

            const tienSauGiam = Math.max(0, tongTienGoc - tienGiamGia);

            // Strategy Pattern: Thue
            let tienThue = 0;
            const maThue = loaiThue || 'VAT_10';
            if (maThue === 'VAT_10') {
                tienThue = tienSauGiam * 0.1;
            } else if (maThue === 'VAT_5') {
                tienThue = tienSauGiam * 0.05;
            } else {
                tienThue = 0;
            }

            const tongTienThanhToan = tienSauGiam + tienThue;

            // Tru ton kho ngay khi tao don
            for (const m of dsMon) {
                const sp = db.sanPham.find(s => s.maSP === m.maSP);
                if (sp) sp.soLuongTon -= m.soLuong;
            }

            const maHoaDon = 'HD' + String(db.hoaDon.length + 1).padStart(2, '0');
            const newHD = {
                maHoaDon,
                ngayTao: layThoiGianHienTai(),
                maKhachHang: maKhachHang || 'ND04',
                tenKhachHang: tenKhachHang || 'Khách vãng lai',
                soDienThoai: soDienThoai || '',
                diaChiGiaoHang: diaChiGiaoHang || '',
                tongTienGoc,
                loaiGiamGia: maGG,
                tienGiamGia,
                loaiThue: maThue,
                tienThue,
                tongTienThanhToan,
                phuongThucThanhToan: phuongThucThanhToan || 'TIEN_MAT',
                trangThaiDon: 'CHO_XAC_NHAN',
                trangThaiThanhToan: phuongThucThanhToan === 'CHUYEN_KHOAN' ? 'DA_THANH_TOAN' : 'CHUA_THANH_TOAN',
                nguoiXuLy: 'Hệ thống Online',
                ghiChu: ghiChu || '',
                danhSachChiTiet: chiTietDon
            };
            db.hoaDon.unshift(newHD);
            kiemTraVaPhatCanhBaoTuDong();

            return json({
                thanhCong: true,
                maHoaDon,
                thongBao: 'Đặt hàng thành công! Đơn hàng đang chờ xác nhận.'
            });
        }

        if (pathname === '/api/don-hang/xac-nhan' && req.method === 'POST') {
            const { maHoaDon, nguoiXuLy } = body;
            const hd = db.hoaDon.find(h => h.maHoaDon === maHoaDon);
            if (!hd) return json({ thanhCong: false, thongBao: 'Không tìm thấy hóa đơn!' }, 400);

            hd.trangThaiDon = 'DA_XAC_NHAN';
            if (nguoiXuLy) hd.nguoiXuLy = nguoiXuLy;
            return json({ thanhCong: true, thongBao: 'Xác nhận đơn hàng thành công!' });
        }

        if (pathname === '/api/don-hang/giao-hang' && req.method === 'POST') {
            const { maHoaDon, trangThaiMoi } = body;
            const hd = db.hoaDon.find(h => h.maHoaDon === maHoaDon);
            if (!hd) return json({ thanhCong: false, thongBao: 'Không tìm thấy hóa đơn!' }, 400);

            hd.trangThaiDon = trangThaiMoi || 'DANG_GIAO';
            return json({ thanhCong: true, thongBao: 'Cập nhật trạng thái đơn hàng thành công!' });
        }

        if (pathname === '/api/don-hang/thanh-toan' && req.method === 'POST') {
            const { maHoaDon } = body;
            const hd = db.hoaDon.find(h => h.maHoaDon === maHoaDon);
            if (!hd) return json({ thanhCong: false, thongBao: 'Không tìm thấy hóa đơn!' }, 400);

            hd.trangThaiThanhToan = 'DA_THANH_TOAN';
            return json({ thanhCong: true, thongBao: 'Xác nhận đã thanh toán đơn hàng!' });
        }

        // Command Pattern: Lenh Huy Hoa Don va Hoan Ton Kho an toan
        if (pathname === '/api/don-hang/huy' && req.method === 'POST') {
            const { maHoaDon } = body;
            const hd = db.hoaDon.find(h => h.maHoaDon === maHoaDon);
            if (!hd) return json({ thanhCong: false, thongBao: 'Không tìm thấy hóa đơn cần hủy!' }, 400);
            if (hd.trangThaiDon === 'DA_HUY') {
                return json({ thanhCong: false, thongBao: 'Đơn hàng này đã bị hủy trước đó!' }, 400);
            }

            const trangThaiCu = hd.trangThaiDon;
            // Hoan lai ton kho
            for (const ct of hd.danhSachChiTiet) {
                const sp = db.sanPham.find(s => s.maSP === ct.maSP);
                if (sp) sp.soLuongTon += ct.soLuong;
            }

            hd.trangThaiDon = 'DA_HUY';

            // Luu vao lich su Command Pattern de co the Undo
            db.commandHistory.push({
                type: 'HUY_DON',
                maHoaDon,
                trangThaiCu,
                chiTiet: hd.danhSachChiTiet.map(c => ({ maSP: c.maSP, soLuong: c.soLuong }))
            });

            kiemTraVaPhatCanhBaoTuDong();
            return json({
                thanhCong: true,
                thongBao: 'Hủy đơn hàng thành công (Command Pattern đã hoàn tồn kho an toàn)!'
            });
        }

        // Command Pattern: Hoan tac (Undo) lenh huy gan nhat
        if (pathname === '/api/don-hang/hoan-tac' && req.method === 'POST') {
            if (db.commandHistory.length === 0) {
                return json({ thanhCong: false, thongBao: 'Không có thao tác nào để hoàn tác!' }, 400);
            }

            const lastCmd = db.commandHistory.pop();
            if (lastCmd.type === 'HUY_DON') {
                const hd = db.hoaDon.find(h => h.maHoaDon === lastCmd.maHoaDon);
                if (hd) {
                    hd.trangThaiDon = lastCmd.trangThaiCu;
                    // Tru nguoc lai so luong ton kho
                    for (const ct of lastCmd.chiTiet) {
                        const sp = db.sanPham.find(s => s.maSP === ct.maSP);
                        if (sp) sp.soLuongTon -= ct.soLuong;
                    }
                    kiemTraVaPhatCanhBaoTuDong();
                    return json({
                        thanhCong: true,
                        thongBao: 'Hoàn tác thành công (Command Undo)!'
                    });
                }
            }
            return json({ thanhCong: false, thongBao: 'Lỗi hoàn tác thao tác!' }, 400);
        }

        // ====================================================
        // BAO CAO THONG KE TONG HOP
        // ====================================================
        if (pathname === '/api/bao-cao' && req.method === 'GET') {
            let tongDoanhThu = 0;
            let donThanhCong = 0;
            let donChoXacNhan = 0;
            let donDaHuy = 0;

            for (const hd of db.hoaDon) {
                if (hd.trangThaiDon === 'DA_GIAO' || hd.trangThaiThanhToan === 'DA_THANH_TOAN') {
                    tongDoanhThu += hd.tongTienThanhToan;
                    donThanhCong++;
                }
                if (hd.trangThaiDon === 'CHO_XAC_NHAN') donChoXacNhan++;
                if (hd.trangThaiDon === 'DA_HUY') donDaHuy++;
            }

            let tongGiaTriTon = 0;
            let soLuongCanhBao = 0;
            for (const sp of db.sanPham) {
                tongGiaTriTon += sp.giaNhap * sp.soLuongTon;
                if (sp.soLuongTon <= sp.tonToiThieu) soLuongCanhBao++;
            }

            return json({
                tongDoanhThu,
                tongDonHang: db.hoaDon.length,
                donThanhCong,
                donChoXacNhan,
                donDaHuy,
                tongSoSanPham: db.sanPham.length,
                tongGiaTriTon,
                soLuongCanhBao,
                soLuongNhapKho: db.phieuNhap.length,
                soLuongKiemKe: db.kiemKe.length
            });
        }

        // 404 cho API khong xac dinh
        return json({ thanhCong: false, thongBao: `API endpoint ${pathname} không tồn tại!` }, 404);

    } catch (err) {
        console.error('API Error:', err);
        return json({ thanhCong: false, thongBao: 'Lỗi máy chủ nội bộ: ' + err.message }, 500);
    }
};
