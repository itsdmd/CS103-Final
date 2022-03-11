# 1. Giới thiệu về MyWords

- MyWords là một trò chơi giải ô chữ chạy trong terminal
- Được lập trình bằng ngôn ngữ C++ và hỗ trợ hệ điều hành Windows
- Mục tiêu của trò chơi: Giải ô chữ bí mật dựa trên một gợi ý cho trước với số lần mở ô chữ gợi ý ít nhất có thể trong khoảng thời gian cho phép
- Chủ đề của các ô chữ trong phiên bản này sẽ xoay quanh những kiến thức phổ thông, đặc biệt là về những chủ đề liên quan đến Việt Nam
- Có 2 dạng gợi ý:
    - ***Cho phép***: Người chơi sẽ được mở một bộ ô chữ (số lượng tùy độ khó) và số điểm sẽ bị giảm đi một nửa
    - ***Cấm***: Mỗi lượt nhận gợi ý loại này, người chơi sẽ bị trừ điểm (tùy độ khó)
- Trò chơi có 3 cấp độ: Dễ - Trung bình - Khó
- Cách tính điểm của trò chơi được miêu tả trong bảng dưới:

| Cấp độ | Thời gian đoán (giây) | Số kí tự của ô chữ | Số kí tự Cho phép | Số kí tự Cấm | Điểm nhận được nếu không nhận gợi ý | Điểm nhận được nếu mở các kí tự Cho phép | Số điểm bị trừ cho mỗi kí tự Cấm | Số điểm bị trừ nếu đoán sai hoặc hết thời gian |
| --- | --- | --- | --- | --- | --- | --- | --- | --- |
| Dễ | 10 | 4 — 8 | 1 | 1 | 10 | 5 | 3 | 2 |
| Trung bình | 15 | 9 — 15 | 2 | 1 | 15 | 8 | 5 | 4 |
| Khó | 20 | 15 — 30 | 3 | 2 | 20 | 10 | 8 | 6 |

# 2. Các chức năng chính

- Hồ sơ của các người chơi (những ô chữ đã chơi, số điểm) sẽ được lưu trữ và quản lí trong một thư mục cục bộ
- Mỗi hồ sơ sẽ được khóa bằng mật khẩu được thiết lập bởi người chơi khi tạo mới hồ sơ
- Menu đăng nhập trực quan để cấp quyền truy cập vào hồ sơ cho người chơi với khả năng điều hướng linh hoạt
- Các ô chữ đã được đoán chính xác sẽ tự động được bỏ qua
- Màn hình terminal sẽ được làm mới tự động để tránh gây rối trong quá trình chơi
- Bảng xếp hạng cho các hồ sơ được lưu

# 3. Chi tiết các module

## Các struct và biến quan trọng

```cpp
/// ------------------------------- STRUCTS -------------------------------- ///

struct BANK {int size; string keys[10], hints[10];};
struct PLAYER {string name; int score; BANK played_e, played_m, played_h;};
struct PLAYER_LIST {int size; PLAYER players[50];};

/// ------------------------------- VARIABLES ------------------------------ ///

BANK bank_e, bank_m, bank_h,         // Ngân hàng câu hỏi
     playing,                        // Được gán giá trị của 1 trong 3 bank_ dựa trên độ khó đã 
     played;                         // Các từ khóa đã chơi của độ khó đã chọn

PLAYER_LIST scrbrd;                  // Scoreboard

PLAYER player;                       // Người chơi

string revealed[30];                 // Các kí tự đã được mở
```

## i. Giao diện người dùng

### a. Initializer()

- **Chức năng:** Khởi động trò chơi
- **Chi tiết:**
    - Gọi hàm *Menu()*
    - Kết nối giữa quá trình đăng nhập với quá trình chơi
    - Báo lỗi và tự động thoát nếu không thể tìm thấy file scoreboard

### b. Menu()

- **Chức năng:** Hỗ trợ người chơi đăng nhập hoặc tạo mới hồ sơ
- **Chi tiết:**
    - Tự động thêm lựa chọn đăng nhập khi file scoreboard có dữ liệu
    - Tạo mới: Gọi hàm *AddNewPlayer()*
    - Đăng nhập: Gọi hàm *PlayerSelector()*

### c. Rule()

- **Chức năng:** In ra bảng cách tính điểm

### d. PlayerSelector()

- **Chức năng:** Đăng nhập vào hồ sơ đã lưu
- **Chi tiết:**
    - Liệt kê tên của tất cả hồ sơ được lưu, sắp xếp theo tên
    - Hỗ trợ điều hướng linh hoạt:
        - Lựa chọn lại hồ sơ khi nhập mật khẩu
        - Quay lại menu chính

### e. PrintScrbrd()

- **Chức năng:** In ra bảng xếp hạng
- **Chi tiết:**
    - In ra tên (và điểm số) của các hồ sơ đã lưu
    - Tự động sắp xếp các hồ sơ theo thứ tự giảm dần về điểm

### f. Printer()

- **Chức năng:** In ra giao diện trong quá trình chơi
- **Chi tiết:**
    - In ra:
        - Tên người chơi
        - Số điểm hiện tại
        - Ô chữ + Gợi ý
    - Làm mới màn hình sau mỗi lượt đoán

### g. AnswerResult()

- **Chức năng:** In ra thông báo về kết quả của câu trả lời của người chơi
- **Chi tiết:** Thông báo với người chơi:
    - Câu trả lời chính xác
    - Câu trả lời không chính xác
    - Thời gian trả lời đã quá giới hạn
    - Số điểm nhận được và bị trừ do lật gợi ý

### h. SessionResult()

- **Chức năng:** Thông báo tổng điểm cuối cùng của người chơi vị trí của người chơi trong bảng xếp hạng
- **Chi tiết:** Gọi hàm *UpdateScrbrdData()* để cập nhật điểm số trong file scoreboard, rồi dựa vào đó để xác định vị trí của người chơi trên bảng xếp hạng

## ii. Đọc - Lưu dữ liệu

### a. ReadKeys()

- **Chức năng:** Đọc dữ liệu từ các file các ngân hàng câu hỏi
- **Chi tiết:** Khởi tạo giá trị cho 3 biến *bank_*

### b. ReadScrbrdData()

- **Chức năng:** Đọc dữ liệu từ file scoreboard
- **Chi tiết:** Khởi tạo giá trị cho biến *scrbrd*

### c. ReadPlayerData (string)

- **Chức năng:** Đọc danh sách các từ đã chơi của người chơi có hồ sơ tên tương ứng với tham trị
- **Chi tiết:** Khởi tạo giá trị cho các mảng *player.played_*

### d. UpdateScrbrdData (string)

- **Chức năng:** Cập nhật danh sách người chơi (nếu hồ sơ mới được tạo) và điểm số vào file scoreboard
- **Chi tiết:** Sử dụng giá trị hiện tại của biến *scrbrd*, sắp xếp danh sách người chơi dựa trên điểm số và ghi đè vào file scoreboard

### e. UpdatePlayerData()

- **Chức năng:** Cập nhật danh sách các từ đã chơi vào hồ sơ của người chơi
- **Chi tiết:** Cập nhật hồ sơ của người chơi với các giá trị hiện tại của các mảng *player.played_*

### f. CreateNewPlayerProfile (string)

- **Chức năng:** Tạo một file hồ sơ mới có tên tương ứng với tham trị

## iii. Quản lí dữ liệu

### a. AddNewPlayer (player)

- **Chức năng:** Tạo hồ sơ mới
- **Chi tiết:**
    - Hỗ trợ người chơi tạo hồ sơ bằng cách nhập tên và mật khẩu
    - Duyệt qua biến *scrbrd*, nếu tên đó đã được sử dụng thì:
        - Hỏi người chơi có muốn khởi tạo lại hồ sơ không
        - Nếu người chơi không đồng ý thì yêu cầu nhập mật khẩu để đăng nhập
            - Có thể quay lại phần nhập tên nếu thay đổi ý định
        - Nếu người chơi đồng ý thì danh sách các từ đã chơi sẽ bị xóa và quay lại điểm số mặc định

### b. UpdateSessionVars()

- **Chức năng:** Cập nhật các biến liên quan trực tiếp đến người chơi
- **Chi tiết:**
    - Cập nhật giá trị cho các biến toàn cục
    - Gán giá trị của ngân hàng từ đã chơi có độ khó tương ứng với lựa chọn của người chơi vào mảng *played*

### c. Encryptor(string)

- **Chức năng:** Mã hóa và giải mã mật khẩu của người dùng
- **Chi tiết:** Sử dụng toán tử XOR để dịch bit của từng kí tự trong chuỗi nhập vào ⇒ vừa mã hóa vừa giải mã

## iv. Thao tác với ô chữ

### a. KeyPicker()

- **Chức năng:** Lựa chọn ngẫu nhiên một câu hỏi chưa được chơi
- **Chi tiết:**
    - Lựa chọn ngẫu nhiên một câu hỏi trong mảng *bank_* tương ứng với độ khó đã chọn
    - Duyệt qua mảng *played* xem câu đó đã được chơi chưa
        - Nếu rồi thì chọn lại
        - Nếu chưa thì:
            - Thêm câu đó vào mảng *played*
            - Trả lại vị trí của câu đó trong mảng *bank_*

### b. AskToReveal()

- **Chức năng:** Hỏi người chơi có muốn mở gợi ý không
- **Chi tiết:**
    - Nếu người chơi muốn mở gợi ý loại *Cho phép*:
        - Trừ điểm
        - Gọi hàm *Revealer()* số lần tương ứng với số kí tự cho phép được mở
    - Nếu đã mở gợi ý loại *Cho phép* và người chơi muốn mở tiếp các gợi ý loại Cấm:
        - Xét xem đã vượt quá số lần cho phép chưa
            - Chưa:
                - Trừ điểm
                - Gọi hàm *Revealer()*
            - Rồi:
                - Buộc người chơi phải đoán

### c. Revealer()

- **Chức năng:** Mở ô chữ
- **Chi tiết:**
    - Chọn ngẫu nhiên một số trong giới hạn của số ô chữ
    - Nếu ô đó đã được lật thì chọn lại
    - Nếu ô đó chưa được lật:
        - Thêm số vừa chọn vào mảng *revealed_pos*
        - Gán kí tự tại vị trí của số vừa chọn của mảng *bank_.keys* vào vị trí tương ứng của mảng *revealed*

### d. Timer()

- **Chức năng:** Đồng hồ đếm ngược

# 4. Chức năng mới

## i. Mật khẩu

- Mỗi hồ sơ sẽ được bảo mật bằng mật khẩu thiết lập bởi người khởi tạo
- Mật khẩu này sẽ được mã hóa trước khi lưu vào file hồ sơ của người chơi
- **Lợi ích:** Không ai có thể dễ dàng thay đổi điểm số của những người chơi khác trong giao diện của trò chơi. Bên cạnh đó cho dù người chơi truy xuất trực tiếp file hồ sơ của người chơi khác cũng không dễ dàng biết được mật khẩu của hồ sơ đó.

## iii. Mã hóa

- Các file dữ liệu được lưu cục bộ được mã hóa tự động giúp tránh gian lận và phát hiện việc chỉnh sửa thủ công
- Các file được mã hóa bao gồm:
    - Các file câu hỏi (mã hoá trực tiếp)
    - File scoreboard (sử dụng khoá mã hoá)

## ii. Menu

- Hỗ trợ quá trình đăng nhập trở nên trực quan hơn
- Giúp người chơi điều hướng dễ dàng, không cần phải chạy lại chương trình nếu mắc lỗi trong quá trình đăng nhập
- Có lựa chọn để hiển thị cách tính điểm

# 5. Hướng phát triển

- Sử dụng giao diện đồ họa giúp tăng tính trực quan và hấp dẫn cho trò chơi
- Sử dụng thêm hình ảnh bên cạnh gợi ý bằng chữ
- Cải thiện đồng hồ đếm ngược
- Hỗ trợ đa nền tảng (Windows/macOS/Linux)
- Lưu và truy cập hồ sơ thông qua cơ sở dữ liệu online
- Cải thiện cơ chế xác định các hồ sơ được lưu của chương trình. Ở phiên bản hiện tại, chương trình dựa hoàn toàn vào file scoreboard để thực hiện tác vụ này, nhưng nên có thêm cách để hỗ trợ quét thông tin đối với các hồ sơ được thêm vào từ nguồn ngoại vi (ví dụ: khi tải dữ liệu từ cơ sở dữ liệu online về folder cục bộ)
