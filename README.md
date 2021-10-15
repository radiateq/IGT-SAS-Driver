# IGT-SAS-Driver
IGT Reno SAS Protocol Implementation

Project was written in 2007 in very limited timeframe on Linux environment.
SAS protocol specifications are difficult and that is stated in documentation it self.
Full compliance with Official SAS Tester has been achieved where required by protocol and possible due to missing hardware or other systems and information.
Tested using virtual COM port. 9-bit hardware UART not tested.
Code should be bug free.

Implemented Features:

This parital implementation of SAS protocol is intended to be completely compatible with SAS specification version 6.02.
Testing has been done using SAS tester. Protocol is responding to SAS Tester as shown in media material. If SAS Client does not state invalid command, command has been processed.
Most of the "device" and game logic behind SAS client is "dummy" logic but meters (variable size) and variables needed are implemented, like configurable games, denominations, tournaments, etc.
As by IGT recommendation we implemented these sections, almost all long polls and other features.:
 - 1, without 1.21 - wake up mode with 9th bit
 - 2.1
 - 2.2
 - 2.3
 - 3.1
 - 3.2
 - partly 3.3 - Synchronization
 - partly 4
 - 5
 - 6.1
 - partly 6.2
 - 7.1
 - 7.2
 - 7.3
 - Sections 8 and 9 are not used by anyone (AFT - Automatic Funds Transfer)
 - 10.1 - implemented with placeholders for logic
 - 11.1
 - 11.3 - not implemented, should send meters - simple task since meters are implemented


Missing Features:

Features missing are all features that require hardware to operate like 9 bit uart, hoppers, reels, etc. For such functionality we implemented SAS logic and placehoders for logic that can be implemented on demand.
Also some features missing finished implementation are those that are missing information manufacturer of the machine that will host SAS Driver, must provide.

When testing using SAS protocol Long Poll functions, all functions respond properly, even if they are not fully implemented except following:
 - functions that are present in SAS Tester but are considered obsolete
 - some functions that have not been properly implemented:
LP0x48 - Send Last Accepted Bill Information - needs actual hardware to keep track of bills
LP0x96-99 - section 11.3 Not implemented, should send meters

- Removed legacy EFT poll 27, request current restricted promotional credits.
- Removed “TBD” polls 0C. OD. 81. 82 and 93 since thev were never defined.




 Below is the guide from "GSA SAS 6 FAQ":

 new Q: Do I need to implement everything in the SAS protocol on my gaming machine?

A: The Interoperability Requirements Specification document, included as part of the SAS 6.02
distribution package, organizes the protocol into functional groups. At a minimum you should
implement everything in Sections 1 and 2.1. Implement Section 2.2 if your gaming machine is
capable of having a player denomination that is different from the accounting denomination.
Implement Section 2.3 if you have any paytables where different wagers can result in a difference
of payback percentage greater than 4%, or otherwise if you choose to or need to support wager
categories. Implement Section 2.4 if you support a bill hopper.

 

Implement Sections 3.2 and 3.3 if you want to be compatible with most ticket in/ticket out
systems. Only implement Section 3.1 if you have a specific customer with a specific need. For
compatibility with legacy bonusing systems, you need to implement Sections 4 and 6.2.
Implement Section 5 if any of your paytables have progressive capability. Implement Sections
6.1, 7.1 and 7.2 if you want to be compatible with modern bonusing and cashless systems. Only
implement Section 7.3 if you need to interface with a debit system. Be sure to consult with the
debit system provider on specific registration needs.


IGT does not support Section 8 or Section 9 on any of our platforms, and it is our understanding
that no SAS systems use these functionalities or have any plans to. For compatibility with many
existing SAS systems it is recommended to support most of Sections 10.1 and 10.3. Most of the
functionality in Section 10.2 is obsolete. Long poll 09 is only used in a very few jurisdictions,
and then needs to be implemented to meet very specific jurisdictional specifications. Consult
with the jurisdictions where you plan to operate for details.

Sections 11.1 and 11.3 contain new functionality that should always be implemented. Section
11.2 should be implemented only on gaming machines capable of interfacing with remote
download and configuration systems for compatibility with systems that require meter change
notification. If implemented, it should be possible to disable this functionality for connection to
systems that to not support it. Sections 11.4, 11.5 and 11.6 should only be implemented on
‘gaming machines that need this functionality.
