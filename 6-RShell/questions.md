1. How does the remote client determine when a command's output is fully received from the server, and what techniques can be used to handle partial reads or ensure complete message transmission?

The remote client determines that a command’s output is fully received by detecting an end of transmission marker (like EOF) or by handling a closed connection. To deal with partial reads, techniques like looping over recv() until the expected data size is received, using non-blocking sockets with select() or implementing application layer acknowledgments can be used.

2. This week's lecture on TCP explains that it is a reliable stream protocol rather than a message-oriented one. Since TCP does not preserve message boundaries, how should a networked shell protocol define and detect the beginning and end of a command sent over a TCP connection? What challenges arise if this is not handled correctly?

Since TCP is a stream-oriented protocol, a networked shell must define explicit message boundaries, such as using delimiters (like null terminator), length prefixed headers or structured formats like JSON. Without proper handling, message fragmentation or concatenation can occur.

3. Describe the general differences between stateful and stateless protocols.

A stateful protocol maintains session information between requests, enabling continuity. A stateless protocol does not store session data between requests, meaning each request is independent. Stateless protocols scale better but require additional mechanisms for tracking sessions when needed.

4. Our lecture this week stated that UDP is "unreliable". If that is the case, why would we ever use it?

UDP is used when low latency is more critical than reliability, such as in real-time applications ( like video streaming). It avoids TCP’s overhead by not guaranteeing delivery, ordering, or retransmissions, making it ideal for fast communication.

5. What interface/abstraction is provided by the operating system to enable applications to use network communications?

The operating system provides sockets as an abstraction for network communication. The socket API allows applications to establish connections, send, and receive data over various network protocols like TCP and UDP.