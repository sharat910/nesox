Nesox: a network resource scheduler leveraging SDN technique
=====


About
-----
This multi-purpose "README" file tends to providing a manual-like explaination on Nesox project:
+ 1. providing overview and background;
+ 2. illustrating operations in detail;


Introduction
------------

Nesox is first developed on OpenFlow 1.3 instance of [HP2920 switch][3] and [Ryu controller framework][2].


Choose a Controller Framework
-----------------------------
Nox controller is currently not supporting OF-v1.3.x, which is actually quite a huge amount of work.
It is a good choice based on which to implement Nesox if one is written in C/C++ whose talent is system (level) programming.
However, nowadays, high-level languages are also used in prototype system software (e.g., distributed systems),
such as Spark in Scala, Jane Street using OCaml, Goldman Sach using Erlang, etc.

Ryu controller framework is relatively a slow runtime, however, its support of growing OpenFlow protocols is nearly the best in open source community.
OpenFlow v1.3.1 is supported by Ryu in the following file "/ryu/ryu/ofproto/ofproto_v1_3_parser.py", 5908 lines of code reflecting its amount of work,
(actually a translation from ONF .c file to .py file fitted into its own framework).

> Currently, Nesox only runs on Ryu.


HP2920 Switch
-------------

### First Things First

**Screen Attaching to Switch**

```Bash
screen /dev/ttyS0 115200
screen -ls
screen -r sockname
screen -X - S sockname quit
```
> Note: sockename is usually the pid (actually, pid.tty.host).

```Bash
screen -S hp2920 /dev/ttyS0 115200
screen -r hp2920
```
> Note: to exit the screen: `Ctr-a: quit`;
> [Screen Quick Reference!][7]


**How to login the switch**

[root@debian0][140917111817][/root]ssh manager@switch

We'd like to keep you up to date about:
  * Software feature updates
  * New product announcements
  * Special events

Please register your products now at:  www.hp.com/networking/register


manager@switch's password:

> HP J9726A 2920-24G Switch
> Software revision **WB.15.14.0007**

Copyright (C) 1991-2014 Hewlett-Packard Development Company, L.P.


                   RESTRICTED RIGHTS LEGEND

	Confidential computer software.  Valid license from HP required for possession,
	use or copying. Consistent with FAR 12.211 and 12.212, Commercial Computer
	Software, Computer Software Documentation, and Technical Data for Commercial
	Items are licensed to the U.S. Government under vendor's standard commercial
	license.
                   HEWLETT-PACKARD DEVELOPMENT COMPANY, L.P.
                   20555 State Highway 249, Houston, TX 77070

Press any key to continue


**OpenFlow v1.3 Support**

Since **K/KA/WB 15.14**, HP Added OpenFlow v1.3 support.
The Switch Software Version is now updated to **WB.15.14.0007** from factory setup.
This update step is ignored here, interested readers could retrieve help from HP website.



**Important OpenFlow features supported by HP2920**
* Multiple Flow tables: **Pipeline processing**
* Auxiliary connections
* Multiple controllers
* Rate-limiting the amount of OpenFlow traffic sent to the controller

**Unsupported OpenFlow features**
* Set-Queue action


### OpenFlow Configuration

A number of configurations of the switch affect the OpenFlow behaviour.
What listed bellow are some important ones, others which are not directly related to the **OpenFlow** are not included.
Please refer to the [HP Switch Website][3] for the readers' own interests.


+ 1. Enable OpenFlow

```Bash
openflow
openflow instance instance-name
openflow instance instance-name enable
```

	HP-2920-24G(of-inst-nesox)# show openflow

	 OpenFlow                     : Enabled
	 IP Control Table Mode        : Disabled

	 Instance Information

	                                                 No. of     No. of     OpenFlow
	  Instance Name                     Oper. Status H/W Flows  S/W Flows  Version
	  --------------------------------- ------------ ---------- ---------- --------
	  nesox                             Down         0          0          1.0

> Note: to delete an openflow instance `no openflow instance instance-name`


+ 2. Configure OpenFlow instances
```Bash
openflow instance { instance-name | aggregate } [ enable | disable ]
```

	HP-2920-24G(config)# openflow instance aggregate
	HP-2920-24G(of-inst-aggregate)# show openflow

	 OpenFlow                     : Enabled
	 IP Control Table Mode        : Disabled

	 Instance Information

	                                                 No. of     No. of     OpenFlow
	  Instance Name                     Oper. Status H/W Flows  S/W Flows  Version
	  --------------------------------- ------------ ---------- ---------- --------
	  aggregate                         Down         0          0          1.0

	HP-2920-24G(of-inst-aggregate)# openflow instance aggregate enable
	A controller must be added to the aggregate instance before enabling it.

> Note: An aggregate instance cannot be created when named instances exist.




+ 3. Configure OpenFlow instance members
```Bash
openflow instance instance-name member vlan vlan-id
```




+ 4. Set OpenFlow instance mode
```Bash
openflow instance { instance-name | aggregate } mode { active | passive }
```



+ 5. Set Flow location
```Bash
openflow instance { instance-name | aggregate } flow-location hardware-only
```
> An error is returned to the controller if the flow cannot be added in hardware and the flow-location is set as hardware-only.


+ 6. Configure software and hardware rate limiting
```Bash
openflow instance instance-name limit { hardware-rate kbps | software-rate pps }
```
> kbps: Default: 0; Range: 0 — 10,000,000;
> pps: Limits the number of packets per second per module that this instance can send to the software path. Default: 100 pps Range: 1 — 10,000 pps;
> Note: Increasing the software rate limit increases CPU consumption and may impact system performance.


+ 7. Configure listener ports: listen for incoming connections from an OpenFlow controller
```Bash
openflow instance { instance-name | aggregate } listen-port [tcp-port] [oobm]
```

+ 8. Configure controller IP and port
```Bash
openflow controller-id <id> [ip <ip-address>] [port <tcp-port>]
controller-interface { vlan <vlan-id> | oobm }
openflow instance { instance-name | aggregate } controller-id controller-ID
```
> A VLAN that is a member of an OpenFlow instance cannot be added as an OpenFlow controller interface.
> When an OpenFlow controller is associated with an OpenFlow instance it cannot be deleted.

+ 9. OpenFlow Monitoring
```Bash
show openflow resources
show openflow controllers
show openflow flow-table
show openflow instance instance-name
show openflow instance instance-name flows
show openflow instance instance-name meters
show openflow instance instance-name limiters
show openflow instance instance-name port-statistics
show openflow instance instance-name message-statistics
show openflow instance instance-name capacities
```

**openflow resources**

	HP-2920-24G(config)# show openflow resources

	 Resource usage in Policy Enforcement Engine

	               |   Rules   |  Rules Used
	         Ports | Available | ACL | QoS | IDM |  OF  | Other |
	 --------------+-----------+-----+-----+-----+------+-------|
	 1-24          |      2032 |   0 |   0 |   0 |    0 |     0 |

	               |   Meters  |  Meters Used
	         Ports | Available | ACL | QoS | IDM |  OF  | Other |
	 --------------+-----------+-----+-----+-----+------+-------|
	 1-24          |       255 |     |   0 |   0 |    0 |     0 |

	               |Application|
	               |Port Ranges|  Application Port Ranges Used
	         Ports | Available | ACL | QoS | IDM |  OF  | Other |
	 --------------+-----------+-----+-----+-----+------+-------|
	 1-24          |        60 |   0 |   0 |   0 |      |     0 |

	 0 of 32 Policy Engine management resources used.

	 Key:
	 ACL = Access Control Lists
	 QoS = Device & Application Port Priority, QoS Policies, ICMP rate limits
	 IDM = Identity Driven Management
	 OF = OpenFlow
	 Other = Management VLAN, DHCP Snooping, ARP Protection, Jumbo IP-MTU,
	         RA Guard, Control Plane Protection.

	 Resource usage includes resources actually in use, or reserved for future
	 use by the listed feature.  Internal dedicated-purpose resources, such as
	 port bandwidth limits or VLAN QoS priority, are not included.

**openflow controllers**

	HP-2920-24G(config)# show openflow controllers

	 Controller Information

	 Controller Id IP Address               Port   Interface
	 ------------- ------------------------ ------ --------------
	 1             202.45.128.170           6633   VLAN 1

**openflow flow-tables**

	HP-2920-24G(config)# show openflow flow-table

	 Flow Table Information

	                                         Refresh
	                                  Max.   Rate      Flow
	 Table Name                       Usage  (seconds) Count
	 -------------------------------- ------ --------- -----
	 IP Control Table                 50%    12        0
	 Policy Engine Table              50%    20        0

	          IP Control Table   Policy Engine Table
	 Slot ID  Current Usage (%)  Current Usage (%)
	 -------- -----------------  -------------------
	 1         0.000000           0.00
	 Note: Current usage is percentage of OpenFlow maximum usage

**openflow instance**

	HP-2920-24G(config)# show openflow instance nesox

	 Configured OF Version    : 1.3
	 Negotiated OF Version    : NA
	 Instance Name            : nesox
	 Admin. Status            : Disabled
	 Member List              : VLAN 100
	 Listen Port              : None
	 Oper. Status             : Down
	 Oper. Status Reason      : NA
	 Datapath ID              : 0000000000000000
	 Mode                     : Active
	 Flow Location            : Hardware and Software
	 No. of Hw Flows          : 0
	 No. of Sw Flows          : 0
	 Hw. Rate Limit           : 0 kbps
	 Sw. Rate Limit           : 100 pps
	 Conn. Interrupt Mode     : Fail-Secure
	 Maximum Backoff Interval : 60 seconds
	 Probe Interval           : 10 seconds
	 Hw. Table Miss Count     : NA
	 No. of Sw Flow Tables    : 1
	 Egress Only Ports        : None
	 Table Model              : NA

	No controllers associated with this instance.


**instance flows**




+ 10. **Configuring number of software flow tables per instance**
```Bash
openflow-instance-name# software-flow-table value
```
> Default: 1, Range: 1–4
> Only applicable for an OpenFlow version 1.3 instance.

+ 11. Setting maximum backoff interval for an instance
```Bash
openflow instance { instance-name | aggregate } max-backoff-interval secs
```
> Default: 60 secondsRange: 1 — 3600 seconds

+ 12. Configuring IP Control Table Mode
```Bash
openflow# ip-control-table-mode
```
> Include IP control table in the OpenFlow packet processing pipeline. Default disabled.

+ 13. Hardware statistics refresh rate
```Bash
openflow-instance-name# hardware statistics | refresh rate policy-engine-table <value>
```
> Refresh rate for policy engine table statistics. Default: 0–3600.

+ 14. Troubleshooting OpenFlow
```Bash
debug openflow <errors|events|instance|packets>
```


Testbed Configuration
---------------------
### The HP2920 Switch

	HP-2920-24G(config)# show system

 	Status and Counters - General System Information

	  System Name        : HP-2920-24G
	  System Contact     :
	  System Location    :

	  MAC Age Time (sec) : 300

	  Time Zone          : 8
	  Daylight Time Rule : None

	  Software revision  : WB.15.14.0007        Base MAC Addr      : c4346b-90c4c0
	  ROM Version        : WB.15.05             Serial Number      : SG45FLWXVL

	  Up Time            : 47 days              Memory   - Total   : 170,250,752
	  CPU Util (%)       : 2                               Free    : 85,696,996

	  IP Mgmt  - Pkts Rx : 317,248              Packet   - Total   : 6750
	             Pkts Tx : 193,942              Buffers    Free    : 5009
	                                                       Lowest  : 4951
	                                                       Missed  : 0


### Cluster Nodes Configuration (hostname-to-IP-to-port)
| Machine Node | IP address | Port No. | Line Color |
|-------------:|------------|:--------:|:----------:|
switch  | 202.45.128.168 | 0 | Red
debian0 | 202.45.128.160 | 0 | White
debian1 | 202.45.128.161 | 1 | White
debian2 | 202.45.128.162 | 2 | Green
debian3 | 202.45.128.163 | 3 | Red
debian4 | 202.45.128.164 | 4 | Yellow
d0      | 202.45.128.170 | 0 |
d1      | 202.45.128.171 | 5 | Red
d2      | 202.45.128.172 | 6 | Green
d3      | 202.45.128.173 | 6 | Blue
d4      | 202.45.128.174 | 8 | Yellow
controller | 202.45.128.169 | 0 | Green

> Note: **Port No.** indicates physical port corresponding to a hardware interface of the switch in this context.
> Port No. = 0 means it is not significant!


### VLAN Configuration

A VLAN is comprised of multiple ports operating as members of the same subnet (broadcast domain).
Ports on multiple devices can belong to the same VLAN, and traffic moving between ports in the same VLAN is bridged (or "switched"). (Traffic moving between different VLANs must be routed.)
A static VLAN is an 802.1Q-compliant VLAN configured with one or more ports that remain members regardless of traffic usage.
(A dynamic VLAN is an 802.1Q-compliant VLAN membership that the switch temporarily creates on a port to provide a link to another port in the same VLAN on another device.)

A group of networked ports assigned to a VLAN form a broadcast domain that is separate from other VLANs that may be configured on the switch.
On a given switch, packets are bridged between source and destination ports that belong to the same VLAN.
Thus, all ports passing traffic for a particular subnet address should be configured to the same VLAN.
Cross-domain broadcast traffic in the switch is eliminated and bandwidth is saved by not allowing packets to flood out all ports.

> The above paragraphs are stealed from HP document titled "HP Switch Software: Advanced Traffic Management Guide (2920 Switches)".
> The sentences are so precise and even beautiful!



**Targeting VLAN Table**

| VLAN ID | VLAN Name | Ports | Bank ID | IP addr |
|---------|:---------:|-------|:-------:|:-------:|
100 | VLAN100 | 1 2 3 4 | 1 | 202.45.128.166
200 | VLAN200 | 5 6 7 8 | 1 | 202.45.128.167


**Initial VLAN Configuration**

	HP-2920-24G(config)# show vlan

	 Status and Counters - VLAN Information

	  Maximum VLANs to support : 256
	  Primary VLAN : DEFAULT_VLAN
	  Management VLAN :

	  VLAN ID Name                             | Status     Voice Jumbo
	  ------- -------------------------------- + ---------- ----- -----
	  1       DEFAULT_VLAN                     | Port-based No    No

> This is the factory default state, in which all ports on the switch belong to the (port-based) default VLAN (DEFAULT_VLAN; VID = 1) and are in the same **broadcast/multicast domain**.
> (The default VLAN is also the Primary VLAN.)


**Detail information of DEFAULT_VLAN**

	HP-2920-24G(config)# show vlans 1

	 Status and Counters - VLAN Information - VLAN 1

	  VLAN ID : 1
	  Name : DEFAULT_VLAN
	  Status : Port-based
	  Voice : No
	  Jumbo : No

	  Port Information Mode     Unknown VLAN Status
	  ---------------- -------- ------------ ----------
	  1                Untagged Learn        Up
	  2                Untagged Learn        Up
	  3                Untagged Learn        Up
	  4                Untagged Learn        Up
	  5                Untagged Learn        Up
	  6                Untagged Learn        Up
	  7                Untagged Learn        Up
	  8                Untagged Learn        Up
	  ...
> Note this list is incomplete!


**Create VLAN 100**

	HP-2920-24G(config)# vlan 100
	HP-2920-24G(vlan-100)# show vlans

	 Status and Counters - VLAN Information

	  Maximum VLANs to support : 256
	  Primary VLAN : DEFAULT_VLAN
	  Management VLAN :

	  VLAN ID Name                             | Status     Voice Jumbo
	  ------- -------------------------------- + ---------- ----- -----
	  1       DEFAULT_VLAN                     | Port-based No    No
	  100     VLAN100                          | Port-based No    No

> Note: use `no vlan VLAN-ID` to delete a specific VLAN.
> A port-based VLAN is layer-2 broadcast domain, while a protocol-based one is layer-3 broadcast domain.
> DEFAULT_VLAN is always present in the switch.


**Asigning Ports to VLANs**

A port can be assigned to several VLANs, but only one of those assignments can be "Untagged".

	HP-2920-24G(config)# show vlans 100

	 Status and Counters - VLAN Information - VLAN 100

	  VLAN ID : 100
	  Name : VLAN100
	  Status : Port-based
	  Voice : No
	  Jumbo : No

	  Port Information Mode     Unknown VLAN Status
	  ---------------- -------- ------------ ----------
	  1                Tagged   Learn        Up
	  2                Tagged   Learn        Up
	  3                Tagged   Learn        Up
	  4                Tagged   Learn        Up


**Show Costom VLAN Information**

```Bash
show vlans custom 1-8 id name:20 ipaddr ipmask ipconfig state status
```

	HP-2920-24G(config)# show vlans custom 1-8 id name:20 ipaddr ipmask ipconfig state status

	Status and Counters - VLAN Information - Custom view

	 VLANID VLAN name            IP Addr         IP Mask         IPConfig   State Status
	 ------ -------------------- --------------- --------------- ---------- ----- ----------
	 1      DEFAULT_VLAN         202.45.128.168  255.255.255.0   Manual     Up    Port-based
	 100    VLAN100                                              Disabled   Up    Port-based



### Controller Configuration



### OpenFlow Instance




Nesox Data Plane
----------------

### Flow Tables (standard mode)
| Table ID. | Function |
|----------:|---------:|
100 | Policy Engine Table
200 | Service Dispatching Table
201 | Dynamic Flow Scheduling Table
202 | Flow Rate Limiting Table
203 | MAC Learning Table
999 | Terminate Processing

> Note: Functional arrangemnts of pipelined flow tables under "IP control table mode" are different and more complicated.
> Nesox currently does not utilize that feature!


Run Nesox Scheduler
-------------------
To run the (ryu-based, maybe nox-based in near future and/or others) nesox scheduler::

	% ryu-manager ryu-nesox.py

This command line will run up the nesox scheduler and print output on screen.




Reference
---------
[Markdown Syntax][1] |
[Ryu SDN Framework][2] |
[HP2920 Switch Series][3] |
[Ryu at Gibhub][4] |
[By liying01][6]

![2920 image][5]




[1]: http://daringfireball.net/projects/markdown/syntax "Markdown"
[2]: http://osrg.github.io/ryu/ "Ryu SDN Framework"
[3]: http://h17007.www1.hp.com/us/en/networking/products/switches/HP_2920_Switch_Series/index.aspx "HP2920 Switch Series"
[4]: https://github.com/osrg/ryu "Ryu at Github"
[5]: ./images/2920_FT.jpg "2920 image"
[6]: liyingcs@hku.hk "liyinghku"
[7]: http://aperiodic.net/screen/quick_reference "screen quick reference"

