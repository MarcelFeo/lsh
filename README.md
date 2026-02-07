# 🐚 LSH — Learning Shell in C

LSH (Learning Shell) é um **shell Unix minimalista escrito em C**, desenvolvido para fins educacionais com foco em **Sistemas Operacionais, Programação de Sistemas e POSIX**.

O projeto implementa funcionalidades básicas de um shell real (como Bash), incluindo execução de processos, built-ins, histórico de comandos e execução em background.

---

## 🚀 Funcionalidades

### ✅ Shell Core
- Leitura de comandos do usuário
- Tokenização (parser simples)
- Execução de programas via `fork()` + `execvp()`
- Controle de processos com `waitpid()`

### 🧩 Built-in Commands
| Comando | Descrição |
|---------|-----------|
| `cd <dir>` | Muda o diretório atual |
| `pwd` | Mostra o diretório atual |
| `echo <msg>` | Imprime texto |
| `clear` | Limpa a tela |
| `history` | Mostra histórico de comandos |
| `help` | Lista comandos internos |
| `exit` | Sai do shell |

### ⚙️ Recursos Avançados
- Prompt colorido com diretório atual
- Histórico de comandos em memória
- Execução em background (`&`)

---

## 🧠 Exemplo de Uso

```bash
lsh:/home/user$ pwd
/home/user

lsh:/home/user$ echo Hello World
Hello World

lsh:/home/user$ sleep 5 &
[bg] process started pid=12345

lsh:/home/user$ history
1 pwd
2 echo Hello World
3 sleep 5 &
