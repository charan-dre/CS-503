1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

_answer here_ The client knows when a command’s output is fully received by checking for an EOF character (`RDSH_EOF_CHAR`), a fixed-length header, or a delimiter like `"END_OF_OUTPUT"`. Since data can arrive in chunks, it must loop over `recv()` until all expected data is received. The best approach for your `dsh` shell is to keep reading until `RDSH_EOF_CHAR` appears, ensuring complete command output.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

_answer here_ Since TCP doesn’t preserve message boundaries, a shell must define them using delimiters, a fixed-length header, or an EOF marker. Without this, commands may split, merge, or block. Clear boundaries ensure reliable execution.

3. Describe the general differences between stateful and stateless protocols.

_answer here_ A stateful protocol remembers past interactions (e.g., TCP, FTP), while a stateless protocol treats each request independently (e.g., HTTP). Stateful is resource-intensive but seamless; stateless is simpler and scales better.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

_answer here_ UDP is used because it’s fast, lightweight, and has low latency since it doesn’t require connection setup or retransmissions like TCP. It's ideal for real-time applications where speed matters more than reliability, such as video streaming, online gaming, VoIP, and DNS. These applications can handle occasional packet loss without needing TCP’s overhead. 

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

_answer here_ The operating system provides the socket API as an abstraction for network communication. It allows applications to create, send, receive, and manage network connections using functions like socket(), bind(), listen(), connect(), send(), and recv(). This enables seamless communication over protocols like TCP and UDP without needing low-level networking details.