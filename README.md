Nesox: network resource scheduler leveraging SDN technique
=====

Introduction
------------

Nesox is first developed on OpenFlow 1.3 instance of HP2920 switch and Ryu controller framework.


Choose a Controller Framework
-----------------------------
Nox controller is currently not supporting OF-v1.3.x, which is actually quite a huge amount of work.
It is a good choice based on which to implement Nesox if one is written in C/C++ whose talent is system (level) programming.
However, nowadays, high-level languages are also used in prototype system software (e.g., distributed systems), such as Spark in Scala, Jane Street using OCaml, Goldman Sach using Erlang, etc.

Ryu controller framework is relatively a slow runtime, however, its support of growing Open Flow protocols is nearly the best in open source community.
Open Flow v1.3.1 is supported by Ryu in the following file "/ryu/ryu/ofproto/ofproto_v1_3_parser.py", 5908 lines of code reflecting its amount of work,
(actually a translation from ONF .c file to .py file fitted into its own framework).


HP2920 Switch
-------------


Run Nesox Scheduler
-------------------
To run the (ryu-based, maybe nox-based in near future and/or others) nesox scheduler::

	% ryu-manager ryu-nesox.py

This command line will run up the nesox scheduler and print output on screen.



Reference
---------
[Ryu SDN Framework](http://osrg.github.io/ryu/)
[HP2920 Switch Series](http://h17007.www1.hp.com/us/en/networking/products/switches/HP_2920_Switch_Series/index.aspx)

