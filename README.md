# **MyShell - 一个简单的 Shell 实现**  

![Shell](https://img.shields.io/badge/Shell-Linux-brightgreen)  
![License](https://img.shields.io/badge/License-MIT-blue)  

## **📌 项目简介**  
**MyShell** 是一个简易的 Unix Shell 实现，支持基本命令执行、I/O 重定向和管道功能。  

---

## **🚀 功能特性**  
✅ **基本命令执行**  
- 支持运行不带参数的外部命令（如 `ls`, `pwd`, `echo` 等）。  

✅ **标准 I/O 重定向**  
- **输入重定向** (`<`)：从文件读取输入，如 `wc < file.txt`  
- **输出重定向** (`>`)：覆盖写入文件，如 `ls > output.txt`  
- **追加输出** (`>>`)：追加写入文件，如 `echo "hello" >> log.txt`  

✅ **管道功能** (`|`)  
- 支持连接两个命令，如 `ls | wc -l`  

✅ **信号处理**  
- 支持 `Ctrl+C`（SIGINT）中断处理  

✅ **退出 Shell**  
- 输入 `exit` 退出  

---

## **🔧 编译与运行**  

### **1. 编译**  
确保已安装 `gcc`，然后执行：  
```bash
make
```
或手动编译：
```bash
gcc my_shell.c -o my_shell.o
```

### **2. 运行**  
```bash
make run
```
或直接运行：
```bash
./my_shell.o
```

### **3. 清理编译文件**  
```bash
make clean
```

### **4. 使用 Docker 运行（Windows）**  
确保已安装 Docker Desktop for Windows，然后按以下步骤操作：  

#### **创建 Dockerfile**  
在项目根目录创建 `Dockerfile`：  
```dockerfile
FROM gcc:latest
WORKDIR /app
COPY . .
RUN make
CMD ["./my_shell.o"]
```

#### **构建镜像**  
```bash
docker build -t myshell .
```

#### **运行容器**  
```bash
docker run -it myshell
```

---

## **📖 使用示例**  

### **1. 基本命令**  
```bash
myshell> ls
myshell> pwd
myshell> echo "Hello, MyShell!"
```

### **2. 输入/输出重定向**  
```bash
myshell> wc < input.txt       # 从文件读取输入
myshell> ls > output.txt      # 输出到文件（覆盖）
myshell> date >> log.txt      # 追加到文件
```

### **3. 管道**  
```bash
myshell> ls | wc -l           # 统计当前目录文件数
```

### **4. 退出 Shell**  
```bash
myshell> exit
```

---

## **⚠️ 已知限制**  
❌ **不支持内置命令**（如 `cd`, `export` 等）  
❌ **不支持后台运行**（`&`）  
❌ **不支持复杂管道**（如 `cmd1 | cmd2 | cmd3`）  

---

## **📜 许可证**  
本项目采用 **MIT License**，详情见 [LICENSE](LICENSE)。  

---

## **📞 反馈与改进**  
如有问题或建议，欢迎提交 Issue 或 PR！  

**Happy Hacking! 🎉**