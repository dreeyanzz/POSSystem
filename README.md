ambot nalang gyud ani nga project sakto pa ba ni among desisyon

Gemini-generated README ahead

# Point of Sale & Inventory Management System

A terminal-based Point of Sale (POS) and inventory management system written in C, featuring role-based access control, real-time inventory tracking, and transaction history management.

## 📋 Overview

This application provides a complete solution for small to medium retail operations, offering separate interfaces for administrators and cashiers. The system uses CSV files as a lightweight database solution and provides an intuitive keyboard-driven interface with ANSI color support.

## ✨ Features

### Admin Features
- **Account Management**
  - Enroll new user and admin accounts
  - Modify account credentials (username, password, display name)
  - Change account status (admin/user)
  - Remove accounts (except the default admin)
  
- **Inventory Management**
  - Add new items with unique identifiers
  - Update item names, stock levels, and prices
  - Remove items from inventory
  - Real-time stock tracking

- **Store Operations**
  - Toggle store status (open/closed)
  - View comprehensive transaction histories across all cashiers
  - Monitor sales and inventory in real-time

### Cashier Features
- **Point of Sale**
  - Quick item scanning with search functionality
  - Real-time stock availability checking
  - Multiple item selection with quantity management
  - Transaction total calculation

- **Transaction History**
  - View personal transaction history
  - Search transactions by item name or identifier
  - Sorted by date (newest first)

### System Features
- **Authentication System**
  - Secure login with username/password
  - Role-based access control (Admin/User)
  - Password masking with toggle visibility

- **Store Status Control**
  - Cashiers can only access the system when store is open
  - Admins have unrestricted access

## 🛠️ Prerequisites

- **C Compiler**: GCC (MinGW for Windows)
- **Operating System**: Windows (uses Windows-specific libraries)
- **Libraries**: 
  - `stdio.h`, `stdlib.h`, `string.h`
  - `conio.h` (for keyboard input)
  - `windows.h` (for Sleep function)
  - `time.h` (for timestamps)

## 📦 Installation

### 1. Clone or Download the Project

```bash
git clone <repository-url>
cd <project-directory>
```

### 2. Compile the Project

Use the provided compilation command:

```bash
gcc main.c Tools\Tools.c Tools\Database.c Pages\AdminPage\AdminPage.c Pages\AdminPage\Subpages\EnrollAccountPage.c Pages\AdminPage\Subpages\AddItemPage.c Pages\AdminPage\Subpages\ItemOperationsPage.c Pages\AdminPage\Subpages\AccountOperationsPage.c Pages\AdminPage\Subpages\TransactionHistoriesPage.c Pages\AdminPage\Subpages\StoreStatusPage.c Pages\UserPage\UserPage.c Pages\UserPage\Subpages\ScanPage.c Pages\UserPage\Subpages\ShowHistoryPage.c -o main
```

Or simply:
```bash
gcc main.c Tools/*.c Pages/AdminPage/*.c Pages/AdminPage/Subpages/*.c Pages/UserPage/*.c Pages/UserPage/Subpages/*.c -o main
```

### 3. Run the Application

```bash
./main
```

## 🚀 Usage

### First-Time Setup

1. **Login with Default Admin Account**
   - Username: `admin`
   - Password: `admin`

2. **Initial Configuration**
   - Add items to inventory
   - Create user accounts for cashiers
   - Set store status to "open" for cashier access

### Navigation Controls

- **Arrow Keys** (`↑` `↓` `←` `→`): Navigate menus and fields
- **Tab**: Move to next field/option
- **Enter**: Confirm selection or submit
- **Escape**: Go back or logout
- **Backspace**: Delete character or reduce quantity
- **=**: Toggle password visibility (on password fields)
- **Home/End**: Jump to first/last item in lists
- **Delete**: Remove selected item (in Scan Page)
- **0-9**: Input numbers for quantities and prices
- **A-Z**: Input text for names and searches

### Admin Workflow

1. **Adding Items**
   - Navigate to "Add Item"
   - Enter item name, identifier, initial stock, and price
   - Press Enter to confirm

2. **Managing Inventory**
   - Use "Item Operations Page" to modify items
   - Search by name or identifier
   - Select operation (rename, change stock, change price, remove)

3. **User Management**
   - Use "Add Account" to create new users
   - Use "Account Operations Page" to modify or remove accounts

4. **Monitoring Sales**
   - "Transaction Histories" shows all sales across cashiers
   - Search by cashier name, item, or date

### Cashier Workflow

1. **Making a Sale**
   - Navigate to "Scan Page"
   - Search for items by name or identifier
   - Press number keys (0-9) to set quantity
   - Press Enter to complete transaction

2. **Viewing History**
   - Navigate to "View Transactions"
   - Search your transaction history
   - View sorted by date

## 📁 Project Structure

```
.
├── main.c                          # Entry point, login system
├── Tools/
│   ├── Tools.h/c                   # Utility functions (ANSI colors, keyboard input, etc.)
│   ├── Database.h/c                # Database operations (CSV management)
│   └── FieldType.h                 # Field type definitions
├── Pages/
│   ├── AdminPage/
│   │   ├── AdminPage.h/c          # Admin main menu
│   │   └── Subpages/
│   │       ├── EnrollAccountPage.c       # Account creation
│   │       ├── AddItemPage.c             # Item creation
│   │       ├── ItemOperationsPage.c      # Item management
│   │       ├── AccountOperationsPage.c   # Account management
│   │       ├── TransactionHistoriesPage.c # Sales reports
│   │       └── StoreStatusPage.c         # Store open/close toggle
│   └── UserPage/
│       ├── UserPage.h/c           # Cashier main menu
│       └── Subpages/
│           ├── ScanPage.c         # POS interface
│           └── ShowHistoryPage.c  # Transaction history
└── Databases/                      # Auto-created CSV databases
    ├── accounts.csv               # User accounts
    ├── items.csv                  # Inventory items
    ├── isOpen.csv                 # Store status
    └── transactions/              # Individual cashier transaction logs
        └── <identifier>.csv
```

## 🗄️ Database Schema

### accounts.csv
```
username|password|displayName|status|identifier
```
- `status`: "admin" or "user"
- `identifier`: 32-character unique ID

### items.csv
```
Item Name|Item Identifier|Item Remaining Stocks|Item Price
```
- All fields required, pipe-delimited

### transactions/<identifier>.csv
```
Item Name|Item Identifier|Stocks Sold|DateTime
```
- One file per cashier
- DateTime format: "Month DD, YYYY"

### isOpen.csv
```
true
```
or
```
false
```

## 🔐 Default Credentials

**Administrator Account:**
- Username: `admin`
- Password: `admin`
- Identifier: `ehyxccqfugqXgM6Q7QXKPChO3iPffF8o`

**Important:** This account cannot be deleted through the UI.

## 🎨 Features in Detail

### Search Functionality
- **Real-time filtering**: Search results update as you type
- **Multi-field search**: Search across names, identifiers, and dates
- **Case-insensitive**: Searches ignore case differences

### Transaction Management
- **Auto-stock deduction**: Stock levels automatically decrease on sale
- **Timestamp tracking**: All transactions are timestamped
- **Sorted history**: Transactions sorted by date (newest first)
- **Per-cashier logs**: Each cashier has their own transaction file

### Inventory Control
- **Duplicate prevention**: Item identifiers must be unique
- **Stock validation**: Cannot sell more than available stock
- **Price management**: Prices stored as integers (in cents or smallest currency unit)

## 🐛 Known Limitations

- Windows-only (uses `conio.h` and `windows.h`)
- CSV-based storage (not suitable for very large datasets)
- No data encryption
- No backup/restore functionality
- Terminal-based UI (no graphical interface)
- Single-user at a time (no concurrent access)

## 🔧 Troubleshooting

### Compilation Errors

**Error: `conio.h` not found**
- Ensure you're using MinGW on Windows
- Linux/Mac users need to replace `conio.h` functions with alternative input methods

**Error: `windows.h` not found**
- This is a Windows-specific application
- To port to Linux/Mac, replace `Sleep()` with `usleep()` or `nanosleep()`

### Runtime Issues

**Databases not created**
- The application automatically creates the `Databases` folder on first run
- Ensure you have write permissions in the application directory

**Store always closed for cashiers**
- Admin must set store status to "open" in Store Status Page
- Check `Databases/isOpen.csv` contains "true"

**Items not appearing in search**
- Verify items exist in `Databases/items.csv`
- Check file format matches schema (pipe-delimited)

## 📝 Development Notes

### What Was Learned (from developer notes)
- `extern` keyword usage
- Modularization techniques
- Unions and structs
- File tokenizing with `strtok()`
- Pointer arithmetic and dereferencing
- Arrow notation for struct access
- File operations (`fopen()`, `fclose()`, `fprintf()`)
- String formatting with `snprintf()`
- Random number generation with `srand()`
- Directory creation with `_mkdir()`
- `size_t` type usage
- Variable-width printf formatting (`%*s`)
- String duplication with `strdup()`
- Static keyword for local scope
- Variadic functions with `stdarg.h`
- `goto` statement usage

## 🤝 Contributing

This appears to be an educational project. If you'd like to enhance it:

1. **Code improvements**
   - Add data validation
   - Implement error handling
   - Add data backup/export features
   - Cross-platform compatibility

2. **Feature additions**
   - Receipt printing
   - Barcode scanner support
   - Sales reports and analytics
   - Multi-currency support
   - Database encryption

3. **UI enhancements**
   - Color customization
   - Better navigation indicators
   - Help screens
   - Keyboard shortcuts reference

## 📄 License

This project appears to be an educational/learning project. Please check with the original authors for licensing information.

## 👥 Authors

Based on the README note in Bisaya (Cebuano):
> "ambot nalang gyud ani nga project sakto pa ba ni among desisyon"
> (Translation: "I don't know about this project anymore, was our decision right?")

This appears to be a student project. The dedication to learning C programming is evident in the comprehensive implementation!

## 🙏 Acknowledgments

- Project uses ANSI escape codes for terminal colors
- CSV format for lightweight data persistence
- Inspired by real-world POS systems

---

**Note**: This is a terminal-based application designed for educational purposes and small-scale retail operations. For production use, consider implementing proper database systems, security measures, and concurrent access controls.
