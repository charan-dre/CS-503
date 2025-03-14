# Exploring Linux User Namespaces

## Introduction

User namespaces in Linux allow unprivileged users to create isolated environments where they appear to have root privileges within the namespace, while remaining a normal user outside of it. This feature is particularly useful for running containers and sandboxing applications without granting full root access on the host system. By remapping user and group IDs (UID/GID), user namespaces provide a secure method for running processes with elevated privileges in a controlled manner.

## Terminal Outputs

1. Examine your current user ID and groups:

```
id
```

Output
```
charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ id
uid=1000(charan) gid=1000(charan) groups=1000(charan),4(adm),20(dialout),24(cdrom),25(floppy),27(sudo),29(audio),30(dip),44(video),46(plugdev),100(users),107(netdev)
```

2. List your current namespaces:

```
ls -la /proc/self/ns/
```

Output
```
charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ ls -la /proc/self/ns/
total 0
dr-x--x--x 2 charan charan 0 Mar 14 00:35 .
dr-xr-xr-x 9 charan charan 0 Mar 14 00:35 ..
lrwxrwxrwx 1 charan charan 0 Mar 14 00:35 cgroup -> 'cgroup:[4026531835]'
lrwxrwxrwx 1 charan charan 0 Mar 14 00:35 ipc -> 'ipc:[4026532244]'
lrwxrwxrwx 1 charan charan 0 Mar 14 00:35 mnt -> 'mnt:[4026532255]'
lrwxrwxrwx 1 charan charan 0 Mar 14 00:35 net -> 'net:[4026531840]'
lrwxrwxrwx 1 charan charan 0 Mar 14 00:35 pid -> 'pid:[4026532257]'
lrwxrwxrwx 1 charan charan 0 Mar 14 00:35 pid_for_children -> 'pid:[4026532257]'
lrwxrwxrwx 1 charan charan 0 Mar 14 00:35 time -> 'time:[4026531834]'
lrwxrwxrwx 1 charan charan 0 Mar 14 00:35 time_for_children -> 'time:[4026531834]'
lrwxrwxrwx 1 charan charan 0 Mar 14 00:35 user -> 'user:[4026531837]'
lrwxrwxrwx 1 charan charan 0 Mar 14 00:35 uts -> 'uts:[4026532256]'
```

3. Create a new user namespace:

```
unshare --user bash
```

Output
```
charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ unshare --user bash
nobody@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ 
```

4. Inside this new bash shell (in the new user namespace), check your user and group IDs:

```
id
```

Output
```
nobody@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ id
uid=65534(nobody) gid=65534(nogroup) groups=65534(nogroup)
```

5. Examine the namespace identifier to confirm you're in a different user namespace:

```
ls -la /proc/self/ns/user
```

Output
```
nobody@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ ls -la /proc/self/ns/user
lrwxrwxrwx 1 nobody nogroup 0 Mar 14 00:38 /proc/self/ns/user -> 'user:[4026532263]'
```

You might also try `whoami` and observe that it prints "nobody".

Output
```
nobody@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ whoami
nobody
```

6. Check your new namespace's UID mapping:

```
cat /proc/self/uid_map
cat /proc/self/gid_map
```
Note that these files are likely empty, indicating no mappings are established.


Output
```
nobody@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ cat /proc/self/uid_map
nobody@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ cat /proc/self/gid_map
nobody@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ 
```

7. Exit the namespace:

```
exit
```

Output

```
nobody@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ exit
exit
charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$
```

## Part 2: Creating User Namespaces with UID/GID Mappings

For this part, we'll use a simple C program to create a user namespace with custom mappings. The C program is `userns_child.c` in the starter folder

Compile and run this program:

```
make
./userns_child
```

Output

```
charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ Current: =ep
Bounding set =cap_chown,cap_dac_override,cap_dac_read_search,cap_fowner,cap_fsetid,cap_kill,cap_setgid,cap_setuid,cap_setpcap,cap_linux_immutable,cap_net_bind_service,cap_net_broadcast,cap_net_admin,cap_net_raw,cap_ipc_lock,cap_ipc_owner,cap_sys_module,cap_sys_rawio,cap_sys_chroot,cap_sys_ptrace,cap_sys_pacct,cap_sys_admin,cap_sys_boot,cap_sys_nice,cap_sys_resource,cap_sys_time,cap_sys_tty_config,cap_mknod,cap_lease,cap_audit_write,cap_audit_control,cap_setfcap,cap_mac_override,cap_mac_admin,cap_syslog,cap_wake_alarm,cap_block_suspend,cap_audit_read,cap_perfmon,cap_bpf,cap_checkpoint_restore
Ambient set =
Current IAB:
Securebits: 00/0x0/1'b0 (no-new-privs=0)
 secure-noroot: no (unlocked)
 secure-no-suid-fixup: no (unlocked)
 secure-keep-caps: no (unlocked)
 secure-no-ambient-raise: no (unlocked)
uid=0(root) euid=0(root)
gid=65534(nogroup)
groups=65534(nogroup),65534(nogroup),65534(nogroup),65534(nogroup),65534(nogroup),65534(nogroup),65534(nogroup),65534(nogroup),65534(nogroup),65534(nogroup),65534(nogroup),65534(nogroup)
Guessed mode: HYBRID (4)
```

Observe the output and answer these questions:

- What is your UID in the new user namespace?
    0

- What capabilities do you have in the user namespace?
    cap_chown, cap_dac_override, cap_dac_read_search, cap_fowner, cap_fsetid, cap_kill, cap_setgid, cap_setuid, cap_setpcap, cap_linux_immutable, cap_net_bind_service, cap_net_broadcast, cap_net_admin, cap_net_raw, cap_ipc_lock, cap_ipc_owner, cap_sys_module, cap_sys_rawio, cap_sys_chroot, cap_sys_ptrace, cap_sys_pacct, cap_sys_admin, cap_sys_boot, cap_sys_nice, cap_sys_resource, cap_sys_time, cap_sys_tty_config, cap_mknod, cap_lease, cap_audit_write, cap_audit_control, cap_setfcap, cap_mac_override, cap_mac_admin, cap_syslog, cap_wake_alarm, cap_block_suspend, cap_audit_read, cap_perfmon, cap_bpf, cap_checkpoint_restore

    Full set of capabilities, but restricted to the namespace.

- How do the UID mappings work?
    The namespace UID 0 is mapped to a non-root UID (e.g., 1000) in the parent namespace, ensuring privilege isolation.

## Part 3: Exploring Isolation with User Namespaces

Run `check_isolation.sh` in the starter folder:

```bash
chmod +x check_isolation.sh
./check_isolation.sh
```

Output

```
charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ chmod +x check_isolation.sh
charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ ./check_isolation.sh
*** BEFORE ENTERING USER NAMESPACE ***
=== Process Information ===
PID: 2456
UID/GID: 1000/1000
Username: charan
Groups: 1000 4 20 24 25 27 29 30 44 46 100 107

=== Namespace Information ===
user namespace: user:[4026531837]
mnt namespace: mnt:[4026532255]
pid namespace: pid:[4026532257]
net namespace: net:[4026531840]
ipc namespace: ipc:[4026532244]
uts namespace: uts:[4026532256]

=== Capability Information ===
Bounding set =cap_chown,cap_dac_override,cap_dac_read_search,cap_fowner,cap_fsetid,cap_kill,cap_setgid,cap_setuid,cap_setpcap,cap_linux_immutable,cap_net_bind_service,cap_net_broadcast,cap_net_admin,cap_net_raw,cap_ipc_lock,cap_ipc_owner,cap_sys_module,cap_sys_rawio,cap_sys_chroot,cap_sys_ptrace,cap_sys_pacct,cap_sys_admin,cap_sys_boot,cap_sys_nice,cap_sys_resource,cap_sys_time,cap_sys_tty_config,cap_mknod,cap_lease,cap_audit_write,cap_audit_control,cap_setfcap,cap_mac_override,cap_mac_admin,cap_syslog,cap_wake_alarm,cap_block_suspend,cap_audit_read,cap_perfmon,cap_bpf,cap_checkpoint_restore

=== Process List (as seen by this process) ===
UID          PID    PPID  C STIME TTY          TIME CMD
root           1       0  0 05:46 ?        00:00:07 /sbin/init
root           2       1  0 05:46 ?        00:00:00 /init
root           7       2  0 05:46 ?        00:00:00 plan9 --control-socket 6 --log-level 4 --server-fd 7 --pipe-fd 9 --log-truncate
root          52       1  0 05:46 ?        00:00:11 /usr/lib/systemd/systemd-journald

*** AFTER ENTERING USER NAMESPACE ***
=== Process Information ===
PID: 2471
UID/GID: 65534/65534
Username: nobody
Groups: 65534

=== Namespace Information ===
user namespace: user:[4026532263]
mnt namespace: mnt:[4026532255]
pid namespace: pid:[4026532257]
net namespace: net:[4026531840]
ipc namespace: ipc:[4026532244]
uts namespace: uts:[4026532256]

=== Capability Information ===
Bounding set =cap_chown,cap_dac_override,cap_dac_read_search,cap_fowner,cap_fsetid,cap_kill,cap_setgid,cap_setuid,cap_setpcap,cap_linux_immutable,cap_net_bind_service,cap_net_broadcast,cap_net_admin,cap_net_raw,cap_ipc_lock,cap_ipc_owner,cap_sys_module,cap_sys_rawio,cap_sys_chroot,cap_sys_ptrace,cap_sys_pacct,cap_sys_admin,cap_sys_boot,cap_sys_nice,cap_sys_resource,cap_sys_time,cap_sys_tty_config,cap_mknod,cap_lease,cap_audit_write,cap_audit_control,cap_setfcap,cap_mac_override,cap_mac_admin,cap_syslog,cap_wake_alarm,cap_block_suspend,cap_audit_read,cap_perfmon,cap_bpf,cap_checkpoint_restore

=== Process List (as seen by this process) ===
UID          PID    PPID  C STIME TTY          TIME CMD
nobody         1       0  0 05:46 ?        00:00:07 /sbin/init
nobody         2       1  0 05:46 ?        00:00:00 /init
nobody         7       2  0 05:46 ?        00:00:00 plan9 --control-socket 6 --log-level 4 --server-fd 7 --pipe-fd 9 --log-truncate
nobody        52       1  0 05:46 ?        00:00:11 /usr/lib/systemd/systemd-journald
```

Note the differences in the output before and after entering the user namespace.

## Part 4: Practical Applications - Running a Program with "Root" Privileges

1. Compile and use it to run commands as "root" (in the namespace):

```
make
./fake_root id
./fake_root bash -c "id && whoami && ls -la /proc/self/ns/"
```

Output
```
charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ make

charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ ./fake_root id
Parent process: UID=1000, GID=1000, PID=2539
Created child process with PID: 2540
Running as UID: 0, GID: 0 (should be 0 for both)
Successfully set hostname in new UTS namespace
uid=0(root) gid=0(root) groups=0(root),65534(nogroup)
Child exited with status: 0

charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ ./fake_root bash -c "id && whoami && ls -la /proc/self/ns/"
Parent process: UID=1000, GID=1000, PID=2541
Created child process with PID: 2542
Running as UID: 0, GID: 0 (should be 0 for both)
Successfully set hostname in new UTS namespace
uid=0(root) gid=0(root) groups=0(root),65534(nogroup)
root
total 0
dr-x--x--x 2 root root 0 Mar 14 14:16 .
dr-xr-xr-x 9 root root 0 Mar 14 14:16 ..
lrwxrwxrwx 1 root root 0 Mar 14 14:16 cgroup -> 'cgroup:[4026531835]'
lrwxrwxrwx 1 root root 0 Mar 14 14:16 ipc -> 'ipc:[4026532244]'
lrwxrwxrwx 1 root root 0 Mar 14 14:16 mnt -> 'mnt:[4026532255]'
lrwxrwxrwx 1 root root 0 Mar 14 14:16 net -> 'net:[4026531840]'
lrwxrwxrwx 1 root root 0 Mar 14 14:16 pid -> 'pid:[4026532257]'
lrwxrwxrwx 1 root root 0 Mar 14 14:16 pid_for_children -> 'pid:[4026532257]'
lrwxrwxrwx 1 root root 0 Mar 14 14:16 time -> 'time:[4026531834]'
lrwxrwxrwx 1 root root 0 Mar 14 14:16 time_for_children -> 'time:[4026531834]'
lrwxrwxrwx 1 root root 0 Mar 14 14:16 user -> 'user:[4026532263]'
lrwxrwxrwx 1 root root 0 Mar 14 14:16 uts -> 'uts:[4026532266]'
Child exited with status: 0
```

Notice that inside the user namespace, your UID is 0 (root), but this is only within the namespace!

2. Try to create a file as "root" in your home directory, using your program:

```
./fake_root touch ~/root_test_file
ls -la ~/root_test_file
```

Output
```
charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ ./fake_root touch ~/root_test_file
Parent process: UID=1000, GID=1000, PID=2583
Created child process with PID: 2584
Running as UID: 0, GID: 0 (should be 0 for both)
Successfully set hostname in new UTS namespace
Child exited with status: 0

charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ ls -la ~/root_test_file
-rw-r--r-- 1 charan charan 0 Mar 14 14:17 /home/charan/root_test_file
```

The file is created, but notice who owns it on the real system!

## Part 5: Exploring the Limitations

1. Try to perform privileged operations in your user namespace:

```
mkdir -p /tmp/test_mount
./fake_root bash -c "mount -t tmpfs none /tmp/test_mount"
```

Output
```
charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ mkdir -p /tmp/test_mount

charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ ./fake_root bash -c "mount -t tmpfs none /tmp/test_mount"
Parent process: UID=1000, GID=1000, PID=2645
Created child process with PID: 2646
Running as UID: 0, GID: 0 (should be 0 for both)
Successfully set hostname in new UTS namespace
mount: /tmp/test_mount: permission denied.
       dmesg(1) may have more information after failed mount system call.
Child exited with status: 32
```

This will likely fail with "Operation not permitted" because of missing capabilities or namespace configurations.

2. Try to access network namespaces (which usually require real root):

```
./fake_root ip link add dummy0 type dummy
```

Output
```
charan@Charan:/mnt/f/Drexel/CS-503/10-Isolation/directions/starter$ ./fake_root ip link add dummy0 type dummy
Parent process: UID=1000, GID=1000, PID=2663
Created child process with PID: 2664
Running as UID: 0, GID: 0 (should be 0 for both)
Successfully set hostname in new UTS namespace
RTNETLINK answers: Operation not permitted
Child exited with status: 2
```

Note and document what errors you encounter.

## Answers to the following questions:
   - How do user namespaces provide the illusion of having root privileges?
        
        User namespaces allow processes inside them to have UID 0 (root), but this is only valid inside the namespace. The kernel maps this root user to an unprivileged user in the parent namespace, enforcing restrictions on actual system resources. This provides the illusion of root privileges while preventing real system-wide administrative access.

   - What is the purpose of UID/GID mapping in user namespaces?
        
        UID/GID mapping ensures that processes inside a user namespace appear as root (or another privileged user) within the namespace while being mapped to an unprivileged user on the host system.

   - What limitations did you encounter when working with user namespaces?
        
        Some common limitations when working with user namespaces include:

        Limited access to system-wide resources:

            Cannot modify host system files (/etc/passwd, /root).
            Cannot load kernel modules (cap_sys_module restricted).
            Cannot access hardware devices.

        Network restrictions:

            By default, user namespaces do not grant access to real network interfaces.
            Must be combined with network namespaces to create isolated networking.
        
        Not all syscalls work:

            Certain privileged operations (e.g., mount --make-shared, sethostname) may not be allowed without additional namespace types.
        
        File system ownership issues:

            Files created in a namespace may still be owned by the unprivileged UID on the host.
            This complicates bind-mounting host directories inside a container.

   - How might user namespaces be used in container technology?
        
        User namespaces provide isolation and security in container runtimes like Docker, LXC, and Podman by allowing containers to run as "root" inside the container but remain unprivileged on the host system.

        Use cases in containers:

            Running unprivileged containers (e.g., rootless Podman).
            Mapping container users to different host users for security.
            Enabling containers to operate without needing sudo or root privileges.

   - What security implications do user namespaces have?
        
        While user namespaces improve security by restricting root privileges, they also introduce potential attack surfaces:

        Privilege escalation risks:

            If misconfigured, a user could escape a namespace and gain real root privileges on the host.
            Some kernel vulnerabilities (e.g., CVE-2017-6074) allowed attackers to exploit namespace mechanisms.

        Increased syscall attack surface:

            User namespaces enable certain system calls (unshare(), setns()) for unprivileged users, potentially leading to new privilege escalation exploits.
        
        File system security risks:

            Improper UID/GID mappings can cause files to be owned by incorrect users on the host.
            Attacks like overlayFS exploits have been linked to misconfigured namespaces.

   - Why are other namespace types typically not available to unprivileged users?
        
        Other namespaces (e.g., network, mount, PID, IPC) affect system-wide resources, making them more dangerous if exposed to unprivileged users.

        Reasons for restricting them:

        Network namespaces:

            Allow creation of virtual interfaces (e.g., bypassing firewall rules, spoofing traffic).
            If unprivileged users could create network namespaces, they might evade monitoring.

        Mount namespaces:

            Could allow unprivileged users to mount sensitive filesystems (/proc, /sys).
            This might be exploited to bypass security restrictions.

        PID namespaces:

            Allow users to manipulate process trees, potentially hiding malicious processes.

        IPC namespaces:

            Could be used to manipulate system-wide inter-process communication, leading to potential security breaches.

        Because of these risks, most Linux distributions restrict these namespaces unless the user has elevated privileges (CAP_SYS_ADMIN).

## A conclusion section with your insights and any challenges you faced.
    User namespaces provide isolation by allowing processes to appear as root within the namespace while being mapped to an unprivileged user outside. They are widely used in containerization, enabling secure rootless environments. However, they come with limitations, such as restricted system-wide access and potential security risks like privilege escalation if misconfigured. Proper UID/GID mapping ensures controlled privilege management. Debugging permission issues and understanding namespace interactions can be challenging. While powerful for security and isolation, they must be carefully configured. User namespaces balance flexibility and security, making them essential in modern computing but requiring vigilance to prevent exploitation.