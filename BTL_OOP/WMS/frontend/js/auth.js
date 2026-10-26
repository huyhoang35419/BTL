// ========================================================
// QUAN LY XAC THUC & PHAN QUYEN (AUTH & ROLES)
// ========================================================

// Live Server chỉ phục vụ frontend (thường ở cổng 5500). Chuyển API sang backend WMS.
// Khi frontend được phục vụ bởi máy chủ C++ ở cổng 8080 hoặc Vercel Serverless API, API dùng cùng origin.
// Người dùng cũng có thể ghi đè API origin bằng cách đặt: localStorage.setItem('wms_api_custom', 'https://your-backend.com')
const customOrigin = (typeof localStorage !== 'undefined' && localStorage.getItem('wms_api_custom')) || '';
const WMS_API_ORIGIN = customOrigin || (window.location.port === '5500' ? 'http://localhost:8080' : '');
if (WMS_API_ORIGIN && !window.__wmsFetchConfigured) {
    const fetchGoc = window.fetch.bind(window);
    window.fetch = (resource, options) => {
        if (typeof resource === 'string' && resource.startsWith('/api/')) {
            return fetchGoc(`${WMS_API_ORIGIN}${resource}`, options);
        }
        return fetchGoc(resource, options);
    };
    window.__wmsFetchConfigured = true;
}

const Auth = {
    layNguoiDungHienTai: function() {
        const u = localStorage.getItem('wms_user');
        return u ? JSON.parse(u) : null;
    },

    layToken: function() {
        return localStorage.getItem('wms_token') || '';
    },

    luuPhienDangNhap: function(user, token) {
        localStorage.setItem('wms_user', JSON.stringify(user));
        localStorage.setItem('wms_token', token);
    },

    dangXuat: function() {
        localStorage.removeItem('wms_user');
        localStorage.removeItem('wms_token');
        window.location.href = 'login.html';
    },

    kiemTraQuyen: function(cacVaiTroChoPhep = []) {
        const user = this.layNguoiDungHienTai();
        if (!user) {
            window.location.href = 'login.html';
            return false;
        }

        if (cacVaiTroChoPhep.length > 0 && !cacVaiTroChoPhep.includes(user.vaiTro)) {
            alert('Bạn không có quyền truy cập chức năng này! Vai trò yêu cầu: ' + cacVaiTroChoPhep.join(', '));
            window.location.href = 'index.html';
            return false;
        }
        return true;
    },

    capNhatGiaoDienNavbar: function() {
        const user = this.layNguoiDungHienTai();
        const navAuth = document.getElementById('nav-auth-container');
        const navMenu = document.getElementById('nav-menu-container');
        const navBrand = document.querySelector('.nav-brand');

        if (!navAuth) return;

        if (user) {
            let roleBadge = '';
            if (user.vaiTro === 'ADMIN') roleBadge = '<span class="badge badge-danger">Admin</span>';
            else if (user.vaiTro === 'QUAN_LY_KHO') roleBadge = '<span class="badge badge-warning">Quản Lý Kho</span>';
            else if (user.vaiTro === 'BAN_HANG') roleBadge = '<span class="badge badge-primary">Bán Hàng</span>';
            else roleBadge = '<span class="badge badge-success">Khách Hàng</span>';

            navAuth.innerHTML = `
                <div class="user-pill">
                    <span>👤 ${user.hoTen}</span>
                    ${roleBadge}
                </div>
                <button onclick="Auth.dangXuat()" class="btn btn-outline btn-sm">Đăng xuất</button>
            `;
        } else {
            navAuth.innerHTML = `
                <a href="login.html" class="btn btn-outline btn-sm">Đăng nhập</a>
                <a href="register.html" class="btn btn-primary btn-sm">Đăng ký</a>
            `;
        }

        const vaiTro = user ? user.vaiTro : 'KHACH';
        const trangTheoVaiTro = {
            ADMIN: 'baocao.html',
            QUAN_LY_KHO: 'kho.html',
            BAN_HANG: 'banhang.html',
            KHACH_HANG: 'index.html',
            KHACH: 'login.html'
        };
        const trangMacDinh = trangTheoVaiTro[vaiTro] || 'index.html';
        if (navBrand) navBrand.href = trangMacDinh;

        const menuTheoVaiTro = {
            ADMIN: [
                ['baocao.html', 'Tổng quan & quản trị'],
                ['baocao.html#phieu-nhap-cho-duyet', 'Duyệt phiếu nhập'],
                ['sanpham.html', 'Danh mục sản phẩm'],
                ['kho.html', 'Tồn kho & duyệt phiếu'],
                ['banhang.html', 'Đơn hàng & vận chuyển']
            ],
            QUAN_LY_KHO: [
                ['kho.html', 'Tồn kho & phiếu nhập'],
                ['sanpham.html', 'Danh mục hàng hóa']
            ],
            BAN_HANG: [
                ['banhang.html', 'Duyệt đơn & vận chuyển'],
                ['hoadon.html', 'Hóa đơn']
            ],
            KHACH_HANG: [
                ['index.html', 'Cửa hàng'],
                ['hoadon.html', 'Đơn hàng của tôi']
            ],
            KHACH: [['index.html', 'Cửa hàng']]
        };

        if (navMenu) {
            const currentPath = window.location.pathname.split('/').pop() || 'index.html';
            navMenu.innerHTML = (menuTheoVaiTro[vaiTro] || menuTheoVaiTro.KHACH)
                .map(([href, label]) => `<li><a href="${href}" class="nav-link${href === currentPath ? ' active' : ''}">${label}</a></li>`)
                .join('');

            if (!user && currentPath === 'index.html') {
                window.location.replace('login.html');
                return;
            }

            // Route nhân viên về màn hình nghiệp vụ khi truy cập thẳng trang cửa hàng.
            if (currentPath === 'index.html' && trangMacDinh !== 'index.html') {
                window.location.replace(trangMacDinh);
            }
        }
    }
};

document.addEventListener('DOMContentLoaded', () => {
    Auth.capNhatGiaoDienNavbar();
});

