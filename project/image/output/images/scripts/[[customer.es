# <- this is for comment / total file size must be less than 4KB
ubi part UBI
tftp 0x21000000 customer.ubifs.0
ubi write.part 0x21000000 customer 0x2300000 0x3168000
tftp 0x21000000 customer.ubifs.1
ubi write.part 0x21000000 customer 0x2300000
% <- this is end of file symbol
