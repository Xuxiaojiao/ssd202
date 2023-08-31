/* drivers/input/touchscreen/hy46xx_ts.c
 *
 * HYCON hy46xx TouchScreen driver.
 *
 * Copyright (c) 2010  Hycon tech Ltd.
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/delay.h>
#include <linux/fs.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/init.h>
#include <linux/timer.h>
#include <linux/input/mt.h>
#include <linux/interrupt.h>
#include <linux/kernel.h>
#include <linux/kthread.h>
#include <linux/mutex.h>
#include <linux/module.h>
#include <linux/mount.h>
#include <linux/netdevice.h>
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/slab.h>
#include <linux/semaphore.h>
#include <linux/syscalls.h>
#include <linux/errno.h>
#include <linux/sched.h>
#include <linux/timer.h>
#include <linux/types.h>
#include <linux/unistd.h>
#include <linux/uaccess.h>
#include <asm/uaccess.h>
#include <asm/unistd.h>
#include <linux/input/mt.h>
#include "hy46xx_ts.h"

#define hy46xx_ts_suspend	NULL
#define hy46xx_ts_resume	NULL

static unsigned char IC_FW_Verson;

//static struct i2c_client *this_client;
//static struct mutex g_device_mutex;
#ifdef HY46XX_FW_UPDATE_ENABLE
/************************************************************/
static int i2c_interface_recv(struct i2c_client* client, unsigned char* pbt_buf, int dw_lenth)
{
    int ret;    
    ret=i2c_master_recv(client, pbt_buf, dw_lenth);
    if(ret<=0)
    {
        return 0;
    }  
    return ret;
}
/************************************************************/
static int i2c_interface_send(struct i2c_client* client, unsigned char* pbt_buf, int dw_lenth)
{
    int ret;
    ret=i2c_master_send(client, pbt_buf, dw_lenth);
    if(ret<0)
    {
        return 0;
    }
    return ret;
}
#endif
/**************************************************************/				
int hy46xx_i2c_Read(struct i2c_client *client, char *writebuf, int writelen, char *readbuf, int readlen)
{
	int ret;

	if (writelen > 0) {
		struct i2c_msg msgs[] = {
			{
			 .addr = client->addr,
			 .flags = 0,
			 .len = writelen,
			 .buf = writebuf,
			 },
			{
			 .addr = client->addr,
			 .flags = 1,
			 .len = readlen,
			 .buf = readbuf,
			 },
		};
		ret = i2c_transfer(client->adapter, msgs, 2);
		if (ret < 0)
			dev_err(&client->dev, "f%s: i2c read error.\n",
				__func__);
	}
	else {
		struct i2c_msg msgs[] = {
			{
			 .addr = client->addr,
			 .flags = 1,
			 .len = readlen,
			 .buf = readbuf,
			 },
		};
		ret = i2c_transfer(client->adapter, msgs, 1);
		
		if (ret < 0)
			dev_err(&client->dev, "%s:i2c read error.\n", __func__);
	}
	
	return ret;
}
/**************************************************************/
int hy46xx_i2c_Write(struct i2c_client *client, char *writebuf, int writelen)
{
	int ret;
	struct i2c_msg msg[] = {
		{
		 .addr = client->addr,
		 .flags = 0,
		 .len = writelen,
		 .buf = writebuf,
		 },
	};
	ret = i2c_transfer(client->adapter, msg, 1);
	if (ret < 0)	
		dev_err(&client->dev, "%s i2c write error.\n", __func__);
	return ret;
}
/**************************************************************/
int hy46xx_write_reg(struct i2c_client * client, u8 regaddr, u8 regvalue)
{
	unsigned char buf[2] = {0};
	buf[0] = regaddr;
	buf[1] = regvalue;
	
	return hy46xx_i2c_Write(client, buf, sizeof(buf));
}
/************************************************************/
int hy46xx_read_reg(struct i2c_client * client, u8 regaddr, u8* regvalue)
{
	return hy46xx_i2c_Read(client, (char *)&regaddr, 1, (char *)regvalue, 1);
}
/***********************************************************
static int hy46xx_read_BootloaderVer(struct i2c_client* client, unsigned char* Boot_Ver)
{
	unsigned char read_cmd[2]= {0xff,0x90};
	unsigned char cmd_len 	= 2;
	unsigned char data_len = 2;
	if(!i2c_interface_send(client, read_cmd, cmd_len))
	{
		return 0;
	}
	msleep(5);
	if(!i2c_interface_recv(client, Boot_Ver, data_len))
	{
		return 0;
	}
}*/
/************************************************************/
void hy46xx_reset_tp(struct hy46xx_ts_data *ts)
{
	printk("%s\n" , __FUNCTION__);
	gpio_direction_output(desc_to_gpio(ts->gpiod_rst), 0);
	msleep(10);
	gpio_direction_output(desc_to_gpio(ts->gpiod_rst), 1);
	msleep(5); 
} 

void hy46xx_reset_for_mode_switch(struct hy46xx_ts_data *ts) {
	gpio_direction_output(desc_to_gpio(ts->gpiod_rst), 0);
	msleep(10);
	gpio_direction_output(desc_to_gpio(ts->gpiod_rst), 1);
	msleep(500); 
}

#ifdef HY46XX_FW_UPDATE_ENABLE
/************************************************************/
int  hys_ctpm_fw_upgrade(struct i2c_client * client, unsigned char* pbt_buf, unsigned long dw_lenth, unsigned char lowbyte)
{
	unsigned char 	ret = 0;
	unsigned char 	Data_Buf[2]  = {0};
	unsigned long 	i = 0, j = 0;

	unsigned long 	packet_number 	 = 0;
	unsigned long  	temp = 0, lenght = 0;

	unsigned char 	packet_buf[133] = {0};
	unsigned char   write_flash_enable_cmd[6] = {0xff, 0x63, 0x03, 0x3d, 0x00, 0x00}; 
	unsigned char	Read_checksum_cmd[6] = {0xff,0x62,0x10,0x00,0xef,0x00};
	unsigned char 	arr_erase_flash[2] = {0xff, 0x91};
	unsigned char  	bt_ecc = 0;
	unsigned char   ae_reg = 0xae;
	unsigned char 	boot_ver_cmd[2] = {0xff, 0x90};
	packet_number = (dw_lenth) / HYS_PACKET_LENGTH;
	
	packet_buf[0] = 0xbf;
	lenght = HYS_PACKET_LENGTH;
	packet_buf[3] = (unsigned char)(lenght >> 8);
	packet_buf[4] = (unsigned char)lenght;
	printk("STEP 1 : RESET TP \n");
	hy46xx_reset_tp();
	msleep(30);
	
	printk("STEP 2 : Read Bootloader version \n");
	ret = i2c_interface_send(client, boot_ver_cmd, 2); 
	ret = i2c_interface_recv(client, Data_Buf, 2);  
	printk("read back: reg_val[0]=0x%x, reg_val[1]=0x%x\n", Data_Buf[0], Data_Buf[1]);	
	
	printk("STEP 3 : Write Flash Enable \n");	
	for(i=0;i<10;i++)
	{
		hy46xx_i2c_Write(client, write_flash_enable_cmd, 6);
		msleep(10);
		i2c_interface_recv(client, Data_Buf, 1);
		printk(" Write Flash Enable value = 0x%x\n", Data_Buf[0]);
		if(Data_Buf[0] == 0x3d)
		{	printk("Write Flash Enable success \n");
			break;
		}
	}	
	if(Data_Buf[0] != 0x3d)
	{
		printk("Update firmware fail in step 3 \n");
		return;
	}
	printk("STEP 4 : Erase Flash \n");
	i2c_interface_send(client, arr_erase_flash, 2);
	msleep(1500);
	printk("STEP 5 : Write Flash \n");
	for (j = 0;j < packet_number; j++) 
	{
		temp = j * HYS_PACKET_LENGTH; 
		packet_buf[1] = (unsigned char)(temp >> 8);  
		packet_buf[2] = (unsigned char)temp;    		
		for (i = 0;i < HYS_PACKET_LENGTH; i++)
		{
		    packet_buf[5 + i] = pbt_buf[j * HYS_PACKET_LENGTH + i]; 		    
		} 				
		ret = i2c_interface_send(client, packet_buf, 133);
		if(ret >= 0)
			printk("wri_in pack_buf is 133: ret=%d , %d\n", ret,j);
		msleep(30);
	}
	printk("STEP 6 : Read Checksum \n");
	ret = i2c_interface_send(client, Read_checksum_cmd, 6);
	msleep(200);
	ret = i2c_interface_recv(client, Data_Buf, 1);
	if((Data_Buf[0] != lowbyte)&&(lowbyte !=0))
	{
		printk("correct ecc is: 0x%x\n", lowbyte);
		printk("ecc return: reg_val[0]=0x%x\n", Data_Buf[0]);	
		printk("ECC fail , Update firmware fail in step 6 \n");
		return -1;
	}
	else
		printk("STEP 7 : Update firmware success ,Reset TP \n");
	
	hy46xx_reset_tp();
	msleep(1000);	
	return 1;	
}
/************************************************************/
unsigned char hys_ctpm_get_i_file_ver(void)
{
    unsigned short ui_sz;
    ui_sz = sizeof(CTPM_FW);
    return CTPM_FW[ui_sz - 1];
}
/************************************************************/
int hys_ctpm_fw_upgrade_with_i_file(struct i2c_client * client)
{
	unsigned char * pbt_buf = NULL;
	int i_ret = -1;
	int i;
	int fw_len = sizeof(CTPM_FW);
	unsigned char IC_FW_Version;
	unsigned char I_File_Version;
	unsigned char uc_reg_addr;
	unsigned char i_checksum = 0;
	
	for(i=0;i<fw_len;i++)
	{
		i_checksum += CTPM_FW[i];
	}
	pbt_buf = CTPM_FW;
	uc_reg_addr = HY46XX_REG_FW_VER;
	hy46xx_reset_tp();
	msleep(2000);
	printk("[TSP] IC firmware version = 0x%02x\n", IC_FW_Version);
	I_File_Version = hys_ctpm_get_i_file_ver();
	printk("[TSP] I file verison = 0x%02x\n", I_File_Version);	
	if((I_File_Version > IC_FW_Version) || (IC_FW_Version == 0xa6))
	{
		do{
			i_ret = hys_ctpm_fw_upgrade(client, pbt_buf, sizeof(CTPM_FW), i_checksum);	
		}while(i_ret<0);
	}				
	return i_ret;
}
#endif 
#ifdef HYS_APK_DEBUG
/************************************************************/
static int hy46xx_GetFirmwareSize(char * firmware_name)
{
	struct file* pfile = NULL;
	struct inode *inode;
	unsigned long magic; 
	off_t fsize = 0; 
	char filepath[128];
	memset(filepath, 0, sizeof(filepath));

	sprintf(filepath, "%s", firmware_name);

	if(NULL == pfile){
		pfile = filp_open(filepath, O_RDONLY, 0);
		}
	if(IS_ERR(pfile)){
		pr_err("error occured while opening file %s.\n", filepath);
		return -EIO;
		}
	inode=pfile->f_dentry->d_inode; 
	magic=inode->i_sb->s_magic;
	fsize=inode->i_size; 
	filp_close(pfile, NULL);
	
	return fsize;
}

/*
*read firmware buf for .bin file.
*/
/************************************************************/
static int hy46xx_ReadFirmware(char * firmware_name, unsigned char * firmware_buf)
{
	struct file* pfile = NULL;
	struct inode *inode;
	unsigned long magic; 
	off_t fsize; 
	char filepath[128];
	loff_t pos;

	mm_segment_t old_fs;
	memset(filepath, 0, sizeof(filepath));
	sprintf(filepath, "%s", firmware_name);
	if(NULL == pfile){
		pfile = filp_open(filepath, O_RDONLY, 0);
		}
	if(IS_ERR(pfile)){
		pr_err("error occured while opening file %s.\n", filepath);
		return -EIO;
		}
	inode=pfile->f_dentry->d_inode; 
	magic=inode->i_sb->s_magic;
	fsize=inode->i_size; 
	old_fs = get_fs();
	set_fs(KERNEL_DS);
	pos = 0;

	vfs_read(pfile, firmware_buf, fsize, &pos);

	filp_close(pfile, NULL);
	set_fs(old_fs);
	return 0;
}

/*
upgrade with *.bin file
*/
/************************************************************/
int hys_ctpm_fw_upgrade_with_app_file(struct i2c_client * client, char * firmware_name)
{
  	unsigned char*     pbt_buf = NULL;
   	int i_ret;
   	int fwsize = hy46xx_GetFirmwareSize(firmware_name);
   	if(fwsize <= 0)
   	{
   		dev_err(&client->dev, "%s ERROR:Get firmware size failed\n", __FUNCTION__);
		return -EIO;
   	}
	if(fwsize<8 || fwsize>64*1024)
	{
		dev_err(&client->dev, "FW length error\n");
		return -EIO;
	}
	
    /*=========FW upgrade========================*/
  	 pbt_buf = (unsigned char *) kmalloc(fwsize+1,GFP_ATOMIC);
	if(hy46xx_ReadFirmware(firmware_name, pbt_buf))
    	{
       	dev_err(&client->dev, "%s() - ERROR: request_firmware failed\n", __FUNCTION__);
        	kfree(pbt_buf);
		return -EIO;
    	}
	if((pbt_buf[fwsize-8]^pbt_buf[fwsize-6])==0xFF
		&& (pbt_buf[fwsize-7]^pbt_buf[fwsize-5])==0xFF
		&& (pbt_buf[fwsize-3]^pbt_buf[fwsize-4])==0xFF)
	{
		hys_ctpm_fw_upgrade(client, pbt_buf, fwsize, 0);			
   		if (i_ret != 0)
   		{
       		dev_err(&client->dev, "%s() - ERROR:[HYS] upgrade failed i_ret = %d.\n",__FUNCTION__,  i_ret);
   		}
   		
		kfree(pbt_buf);
	}
	else
	{
		dev_dbg(&client->dev, "FW format error\n");
		kfree(pbt_buf);
		return -EIO;
	}
   	return i_ret;
}
/************************************************************/
static ssize_t hy46xx_tpfwver_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	ssize_t num_read_chars = 0;
	unsigned char	   fwver = 0;
	struct i2c_client *client = container_of(dev, struct i2c_client, dev);
	
	mutex_lock(&g_device_mutex);
	if(hy46xx_read_reg(client, HY46XX_REG_FW_VER, &fwver) < 0)
		num_read_chars = snprintf(buf, PAGE_SIZE, "get tp fw version fail!\n");
	else
		num_read_chars = snprintf(buf, PAGE_SIZE, "%02X\n", fwver);

	mutex_unlock(&g_device_mutex);
	return num_read_chars;
}
/************************************************************/
static ssize_t hy46xx_tpfwver_store(struct device *dev,
					struct device_attribute *attr,
						const char *buf, size_t count)
{
	/* place holder for future use */
	return -EPERM;
}
/************************************************************/
static ssize_t hy46xx_tprwreg_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	/* place holder for future use */
	return -EPERM;
}
/************************************************************/
static ssize_t hy46xx_tprwreg_store(struct device *dev,
					struct device_attribute *attr,
						const char *buf, size_t count)
{
	struct i2c_client *client = container_of(dev, struct i2c_client, dev);	
	ssize_t num_read_chars = 0;
	int retval;
	unsigned long wmreg=0;
	unsigned char regaddr=0xff,regvalue=0xff;
	unsigned char valbuf[5]={0};
	memset(valbuf, 0, sizeof(valbuf));
	
	mutex_lock(&g_device_mutex);
	num_read_chars = count - 1;

	if(num_read_chars!=2)
	{
		if(num_read_chars!=4)
		{
			dev_err(dev, "please input 2 or 4 character\n");
			goto error_return;
		}
	}
	
	memcpy(valbuf, buf, num_read_chars);
	retval = strict_strtoul(valbuf, 16, &wmreg);
	if (0 != retval)
	{
    	dev_err(dev, "%s() - ERROR: Could not convert the given input to a number. The given input was: \"%s\"\n", __FUNCTION__, buf);
    	goto error_return;
	}

	if(2 == num_read_chars)
	{
		//read register
		regaddr = wmreg;
		if(hy46xx_read_reg(client, regaddr, &regvalue) < 0)
			dev_err(dev, "Could not read the register(0x%02x)\n", regaddr);
		else
			dev_dbg(dev, "the register(0x%02x) is 0x%02x\n", regaddr, regvalue);
	}
	else
	{
		regaddr = wmreg>>8;
		regvalue = wmreg;
		if(hy46xx_write_reg(client, regaddr, regvalue)<0)
			dev_err(dev, "Could not write the register(0x%02x)\n", regaddr);
		else
			dev_dbg(dev, "Write 0x%02x into register(0x%02x) successful\n", regvalue, regaddr);
	}
error_return:
	mutex_unlock(&g_device_mutex);

	return count;
}
/************************************************************/
static ssize_t hy46xx_fwupdate_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	/* place holder for future use */
    return -EPERM;
}
/************************************************************/
static ssize_t hy46xx_fwupdate_store(struct device *dev,
					struct device_attribute *attr,
						const char *buf, size_t count)
{
	struct hy46xx_ts_data *data = NULL;
	unsigned char uc_host_fm_ver;int i_ret;
	struct i2c_client *client = container_of(dev, struct i2c_client, dev);
	data = (struct hy46xx_ts_data *) i2c_get_clientdata( client );
	
	mutex_lock(&g_device_mutex);

//	disable_irq(client->irq);
	i_ret = hys_ctpm_fw_upgrade_with_i_file(client);    
	if (i_ret == 0)
	{
	    msleep(300);
	    uc_host_fm_ver = hys_ctpm_get_i_file_ver();
	    dev_dbg(dev, "%s [HYS] upgrade to new version 0x%x\n", __FUNCTION__, uc_host_fm_ver);
	}
	else
	{
	    dev_err(dev, "%s ERROR:[HYS] upgrade failed ret=%d.\n", __FUNCTION__, i_ret);
	}
//	enable_irq(client->irq);
	
	mutex_unlock(&g_device_mutex);

	return count;
}
/************************************************************/
static ssize_t hy46xx_fwupgradeapp_show(struct device *dev,
				struct device_attribute *attr, char *buf)
{
	/* place holder for future use */
    return -EPERM;
}
/************************************************************/
static ssize_t hy46xx_fwupgradeapp_store(struct device *dev,
					struct device_attribute *attr,
						const char *buf, size_t count)
{
	char fwname[128];
	struct i2c_client *client = container_of(dev, struct i2c_client, dev);
	memset(fwname, 0, sizeof(fwname));
	sprintf(fwname, "%s", buf);
	fwname[count-1] = '\0';

	mutex_lock(&g_device_mutex);
//	disable_irq(client->irq);
	
	hys_ctpm_fw_upgrade_with_app_file(client, fwname);
	
//	enable_irq(client->irq);

	mutex_unlock(&g_device_mutex);

	return count;
}
static ssize_t hy46xx_hysgetprojectcode_show(struct device *dev, struct device_attribute *attr, char *buf)
{
	ssize_t num_read_chars = 0;
	char projectcode[32]; 
	struct i2c_client *client = container_of(dev, struct i2c_client, dev);

	memset(projectcode, 0, sizeof(projectcode));
	mutex_lock(&g_device_mutex);
	//if(hycon_read_project_code(client, projectcode) < 0)
	if(1)
		num_read_chars = snprintf(buf, PAGE_SIZE, "get projcet code fail!\n");
	else
		num_read_chars = snprintf(buf, PAGE_SIZE, "projcet code = %s\n", projectcode);

	mutex_unlock(&g_device_mutex);
	return num_read_chars;
}
/************************************************************/
static ssize_t hy46xx_hysgetprojectcode_store(struct device *dev, struct device_attribute *attr, const char *buf, size_t count)
{
	/* place holder for future use */
    return -EPERM;
}
static DEVICE_ATTR(hystpfwver, S_IRUGO|S_IWUSR, hy46xx_tpfwver_show, hy46xx_tpfwver_store);

static DEVICE_ATTR(hysfwupdate, S_IRUGO|S_IWUSR, hy46xx_fwupdate_show, hy46xx_fwupdate_store);

static DEVICE_ATTR(hystprwreg, S_IRUGO|S_IWUSR, hy46xx_tprwreg_show, hy46xx_tprwreg_store);

static DEVICE_ATTR(hysfwupgradeapp, S_IRUGO|S_IWUSR, hy46xx_fwupgradeapp_show, hy46xx_fwupgradeapp_store);

static DEVICE_ATTR(hysgetprojectcode, S_IRUGO|S_IWUSR, hy46xx_hysgetprojectcode_show, hy46xx_hysgetprojectcode_store);

static struct attribute *hy46xx_attributes[] = {
	&dev_attr_hystpfwver.attr,
	&dev_attr_hysfwupdate.attr,
	&dev_attr_hystprwreg.attr,
	&dev_attr_hysfwupgradeapp.attr,
	&dev_attr_hysgetprojectcode.attr,
	NULL
};

static struct attribute_group hy46xx_attribute_group = {
	.attrs = hy46xx_attributes
};
/************************************************************/
int hy46xx_create_sysfs(struct i2c_client * client)
{
	int err;
	err = sysfs_create_group(&client->dev.kobj, &hy46xx_attribute_group);
   	if (0 != err)
  	{
		dev_err(&client->dev, "%s() - ERROR: sysfs_create_group() failed.error code: %d\n", __FUNCTION__, err);
		sysfs_remove_group(&client->dev.kobj, &hy46xx_attribute_group);
		return -EIO;
  	}
   	else
    	{		
		mutex_init(&g_device_mutex);
        	dev_dbg(&client->dev, "hy46xx:%s() - sysfs_create_group() succeeded. \n", __FUNCTION__);
    	}
	return err;
}
/************************************************************/
int hy46xx_remove_sysfs(struct i2c_client * client)
{
	sysfs_remove_group(&client->dev.kobj, &hy46xx_attribute_group);
	mutex_destroy(&g_device_mutex);
}
/************************************************************/
static int hy46xx_debug_write(struct file *filp, 
	const char __user *buff, unsigned long len, void *data)
{
	struct i2c_client *client = (struct i2c_client *)hy46xx_proc_entry->data;
	unsigned char writebuf[HYS_PACKET_LENGTH];
	int buflen = len;
	int writelen = 0;
	int ret = 0;
	
	if (copy_from_user(&writebuf, buff, buflen)) {
		dev_err(&client->dev, "%s:copy from user error\n", __func__);
		return -EFAULT;
	}
	proc_operate_mode = writebuf[0];
	
	switch (proc_operate_mode) {
	case PROC_UPGRADE:
		{
			char upgrade_file_path[128];
			memset(upgrade_file_path, 0, sizeof(upgrade_file_path));
			sprintf(upgrade_file_path, "%s", writebuf + 1);
			upgrade_file_path[buflen-1] = '\0';
			//DBG("%s\n", upgrade_file_path);
//			disable_irq(client->irq);

			ret = hys_ctpm_fw_upgrade_with_app_file(client, upgrade_file_path);

//			enable_irq(client->irq);
			if (ret < 0) {
				dev_err(&client->dev, "%s:upgrade failed.\n", __func__);
				return ret;
			}
		}
		break;
	case PROC_READ_REGISTER:
		writelen = 1;
		//DBG("%s:register addr=0x%02x\n", __func__, writebuf[1]);
		ret = hy46xx_i2c_Write(client, writebuf + 1, writelen);
		if (ret < 0) {
			dev_err(&client->dev, "%s:write iic error\n", __func__);
			return ret;
		}
		break;
	case PROC_WRITE_REGISTER:
		writelen = 2;
		ret = hy46xx_i2c_Write(client, writebuf + 1, writelen);
		if (ret < 0) {
			dev_err(&client->dev, "%s:write iic error\n", __func__);
			return ret;
		}
		break;
	
	default:
		break;
	}
	
	return len;
}

/************************************************************/
static int hy46xx_debug_read( char *page, char **start,
	off_t off, int count, int *eof, void *data )
{
	struct i2c_client *client = (struct i2c_client *)hy46xx_proc_entry->data;
	int ret = 0, err = 0;
	unsigned char tx = 0, rx = 0;
	int i, j;
	unsigned char buf[PAGE_SIZE];
	int num_read_chars = 0;
	int readlen = 0;
	unsigned char regvalue = 0x00, regaddr = 0x00;
	switch (proc_operate_mode) {
	case PROC_UPGRADE:
		/*after calling hy46xx_debug_write to upgrade*/
		regaddr = 0xA6;
		ret = hy46xx_read_reg(client, regaddr, &regvalue);
		if (ret < 0)
			num_read_chars = sprintf(buf, "%s", "get fw version failed.\n");
		else
			num_read_chars = sprintf(buf, "current fw version:0x%02x\n", regvalue);
		break;
	case PROC_READ_REGISTER:
		readlen = 1;
		ret = hy46xx_i2c_Read(client, NULL, 0, buf, readlen);
		if (ret < 0) {
			dev_err(&client->dev, "%s:read iic error\n", __func__);
			return ret;
		} else
			//DBG("%s:value=0x%02x\n", __func__, buf[0]);
		num_read_chars = 1;
		break;
	default:
		break;
	}
	
	memcpy(page, buf, num_read_chars);

	return num_read_chars;
}

/************************************************************/
int hy46xx_create_apk_debug_channel(struct i2c_client * client)
{
	hy46xx_proc_entry = create_proc_entry(PROC_NAME, 0777, NULL);
	if (NULL == hy46xx_proc_entry) {
		dev_err(&client->dev, "Couldn't create proc entry!\n");
		return -ENOMEM;
	} else {
		dev_info(&client->dev, "Create proc entry success!\n");
		hy46xx_proc_entry->data = client;
		hy46xx_proc_entry->write_proc = hy46xx_debug_write;
		hy46xx_proc_entry->read_proc = hy46xx_debug_read;
	}
	return 0;
}
/************************************************************/
void hy46xx_release_apk_debug_channel(void)
{
	if (hy46xx_proc_entry)
		remove_proc_entry(PROC_NAME, NULL);
}

static int hy_rw_iic_drv_major = HY_RW_IIC_DRV_MAJOR;
struct hy_rw_i2c_dev {
	struct cdev cdev;
	struct mutex hy_rw_i2c_mutex;
	struct i2c_client *client;
};

struct hy_rw_i2c_dev *hy_rw_i2c_dev_tt;
static struct class *hys_class;
/************************************************************/
static int iic_drv_myread(struct i2c_client *client, unsigned char *buf, int length)
{
	int ret = 0;	
	ret = hy46xx_i2c_Read(client, NULL, 0, buf, length);	
	if(ret<0)
		dev_err(&client->dev, "%s:IIC Read failed\n",__func__);
    return ret;   
}
/************************************************************/
static int iic_drv_mywrite(struct i2c_client *client, unsigned char *buf, int length)
{
	int ret = 0;
	ret = hy46xx_i2c_Write(client, buf, length);
	if(ret<0)
		dev_err(&client->dev, "%s:IIC Write failed\n",__func__);
	return ret; 
}
/************************************************************/
void hy_rw_iic_drv_fwupdate(struct i2c_client *client, unsigned long arg)
{
	struct hy_rw_i2c_queue1 i2c_rw_queue1;
	struct hy_rw_i2c1* p;
	unsigned char *p1;
	unsigned char* p_backup;
	unsigned long num = 0;
	unsigned long int i_checksum = 0;
	int ret = 0;
	int i;	
	unsigned char  isum_lowbyte = 0;
	if (!access_ok(VERIFY_READ, (struct hy_rw_i2c_queue1*)arg, sizeof(struct hy_rw_i2c_queue1)))
		return -EFAULT;
	
	if(copy_from_user(&i2c_rw_queue1, (struct hy_rw_i2c_queue1*)arg,sizeof(struct hy_rw_i2c_queue1)))
		return -EFAULT;
	num = i2c_rw_queue1.num;
	
	p = (struct hy_rw_i2c1*)kmalloc(sizeof(struct hy_rw_i2c1), GFP_KERNEL);
	ret = copy_from_user(p, i2c_rw_queue1.i2c_queue1,sizeof(struct hy_rw_i2c1));
	
	p_backup = p->bytearr_native;

	for(i = 0; i < num; i++) {
		i_checksum += p_backup[i];
	}    
	isum_lowbyte = (unsigned char) i_checksum;
	 
	hys_ctpm_fw_upgrade(client, p->bytearr_native, num, isum_lowbyte);
		
	kfree(p);
	p_backup = NULL;
	printk("--after kfree\n");

}
/************************************************************/
void hy_rw_iic_drv_RDWR(struct i2c_client *client, unsigned long arg)
{
	struct hy_rw_i2c_queue i2c_rw_queue;
	unsigned char __user **data_ptrs;
	struct hy_rw_i2c * i2c_rw_msg;
	int ret = 0;
	int i;
			
	if (!access_ok(VERIFY_READ, (struct hy_rw_i2c_queue *)arg, sizeof(struct hy_rw_i2c_queue)))
		return -EFAULT;
		
	if (copy_from_user(&i2c_rw_queue,(struct hy_rw_i2c_queue *)arg, sizeof(struct hy_rw_i2c_queue)))
		return -EFAULT;		
	
	if (i2c_rw_queue.queuenum > HY_I2C_RDWR_MAX_QUEUE)
		return -EINVAL;
	i2c_rw_msg = (struct hy_rw_i2c*)
	kmalloc(i2c_rw_queue.queuenum *sizeof(struct hy_rw_i2c),GFP_KERNEL);
	
	if (!i2c_rw_msg)
		return -ENOMEM;

	if (copy_from_user(i2c_rw_msg, i2c_rw_queue.i2c_queue,i2c_rw_queue.queuenum*sizeof(struct hy_rw_i2c))) {
		kfree(i2c_rw_msg);
		return -EFAULT;
	}
	data_ptrs = kmalloc(i2c_rw_queue.queuenum * sizeof(unsigned char __user *), GFP_KERNEL);
	if (data_ptrs == NULL) {
		kfree(i2c_rw_msg);
		return -ENOMEM;
	}
	
	for (i=0; i< i2c_rw_queue.queuenum; i++) {
		if ((i2c_rw_msg[i].length > 8192)||(i2c_rw_msg[i].flag & I2C_M_RECV_LEN)) {
			ret = -EINVAL;
			break;
		}
		data_ptrs[i] = (unsigned char __user *)i2c_rw_msg[i].buf;
		i2c_rw_msg[i].buf = kmalloc(i2c_rw_msg[i].length, GFP_KERNEL);
		if (i2c_rw_msg[i].buf == NULL) {
			ret = -ENOMEM;
			break;
		}

		if (copy_from_user(i2c_rw_msg[i].buf, data_ptrs[i], i2c_rw_msg[i].length)) {
			++i;
			ret = -EFAULT;
			break;
		} 
	}

	if (ret < 0) {
		int j;
		for (j=0; j<i; ++j){
			kfree(i2c_rw_msg[j].buf);}
		kfree(data_ptrs);
		kfree(i2c_rw_msg);
	}
	for (i=0; i< i2c_rw_queue.queuenum; i++) {
		if (i2c_rw_msg[i].flag) {							
   	   		ret = iic_drv_myread(client,i2c_rw_msg[i].buf, i2c_rw_msg[i].length);
			if (ret>=0) {
   	   			ret = copy_to_user(data_ptrs[i], i2c_rw_msg[i].buf, i2c_rw_msg[i].length);
   	   			kfree(i2c_rw_msg[i].buf);
   	   		}
   	   	}
		else {
			iic_drv_mywrite(client,
					i2c_rw_msg[i].buf, i2c_rw_msg[i].length);	
					kfree(i2c_rw_msg[i].buf);				
		}	
	}
	
	kfree(data_ptrs);
	kfree(i2c_rw_msg);
	
}
/************************************************************/
static int hy_rw_iic_drv_open(struct inode *inode, struct file *filp)
{
	filp->private_data = hy_rw_i2c_dev_tt;
	return 0;
}
/************************************************************/
static int hy_rw_iic_drv_release(struct inode *inode, struct file *filp)
{
	return 0;
}
/************************************************************/
void hy_rw_iic_drv_ioctl(struct file *filp, unsigned
  int cmd, unsigned long __user arg)
{
	struct hy_rw_i2c_dev *ftdev = filp->private_data;
	ftdev = filp->private_data;
	mutex_lock(&hy_rw_i2c_dev_tt->hy_rw_i2c_mutex);
	switch (cmd)
	{
	case HY_I2C_RW: 
		hy_rw_iic_drv_RDWR(ftdev->client, arg);	
		break;
	case HY_UPDATE_FW:
		hy_rw_iic_drv_fwupdate(ftdev->client, arg);
		hy46xx_reset_tp(ftdev->client);
		break;
	case HY_BOOT_VER:
		//get_boot_ver(ftdev->client);
		break;
	case HY_CMD_ResetTP:
		hy46xx_reset_tp(ftdev->client);
		break;
	default:
		break;
	}
	mutex_unlock(&hy_rw_i2c_dev_tt->hy_rw_i2c_mutex);
}
static const struct file_operations hy_rw_iic_drv_fops = {
	.owner			= THIS_MODULE,
	.open			= hy_rw_iic_drv_open,
	.release		= hy_rw_iic_drv_release,
	.unlocked_ioctl	= hy_rw_iic_drv_ioctl,
};
/************************************************************/
static void hy_rw_iic_drv_setup_cdev(struct hy_rw_i2c_dev *dev, int index)
{
	int err, devno = MKDEV(hy_rw_iic_drv_major, index);

	cdev_init(&dev->cdev, &hy_rw_iic_drv_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &hy_rw_iic_drv_fops;
	err = cdev_add(&dev->cdev, devno, 1);
	if (err)
		printk(KERN_NOTICE "Error %d adding LED%d", err, index);
}
/************************************************************/
static int hy_rw_iic_drv_myinitdev(struct i2c_client *client)
{
	int err = 0;
	dev_t devno = MKDEV(hy_rw_iic_drv_major, 0);

	if (hy_rw_iic_drv_major)
		err = register_chrdev_region(devno, 1, HY_RW_IIC_DRV);
	else {
		err = alloc_chrdev_region(&devno, 0, 1, HY_RW_IIC_DRV);
		hy_rw_iic_drv_major = MAJOR(devno);
	}
	if (err < 0) {
		dev_err(&client->dev, "%s:hy_rw_iic_drv failed  error code=%d---\n",
				__func__, err);
		return err;
	}

	hy_rw_i2c_dev_tt = kmalloc(sizeof(struct hy_rw_i2c_dev), GFP_KERNEL);
	if (!hy_rw_i2c_dev_tt){
		err = -ENOMEM;
		unregister_chrdev_region(devno, 1);
		dev_err(&client->dev, "%s:hy_rw_iic_drv failed\n",__func__);
		return err;
	}
	hy_rw_i2c_dev_tt->client = client;
	
	mutex_init(&hy_rw_i2c_dev_tt->hy_rw_i2c_mutex);
	hy_rw_iic_drv_setup_cdev(hy_rw_i2c_dev_tt, 0); 

	hys_class = class_create(THIS_MODULE, "hys_class");
	if (IS_ERR(hys_class)) {
		dev_err(&client->dev, "%s:failed in creating class.\n",
				__func__);
		return -1; 
	} 
	device_create(hys_class, NULL, MKDEV(hy_rw_iic_drv_major, 0), 
			NULL, HY_RW_IIC_DRV);

	return 0;
}
/************************************************************/
int hy_rw_iic_drv_init(struct i2c_client *client)
{
	return hy_rw_iic_drv_myinitdev(client);
}
/************************************************************/
void  hy_rw_iic_drv_exit(void)
{
	device_destroy(hys_class, MKDEV(hy_rw_iic_drv_major, 0)); 
	class_destroy(hys_class); 
	cdev_del(&hy_rw_i2c_dev_tt->cdev);
	kfree(hy_rw_i2c_dev_tt);
	unregister_chrdev_region(MKDEV(hy_rw_iic_drv_major, 0), 1); 
}
#endif

#ifdef HYS_READ_RAWDATA
struct hy_user_data {
    char mode;
    char cmd1[4];
    char cmd2[6];
    char cmd3[4];
    char cmd4[6];
};

struct hy_rawdata {
	char mode;
	char tx, rx;
//	unsigned char ver;
    short rawdata[26][15];
};

struct hy_rw_i2c_dev {
	struct cdev cdev;
	struct mutex hy_rw_i2c_mutex;
	struct i2c_client *client;

	struct hy_user_data user_data;

	struct hy_rawdata rawdata;

	wait_queue_head_t read_queue;
	int unread;
};

static int hy_rw_iic_drv_major = HY_RW_IIC_DRV_MAJOR;

struct hy_rw_i2c_dev *hy_rw_i2c_dev_tt;
static struct class *hys_class;

static struct task_struct* thread = NULL;
static int threadRunningStatus = 0;

// file_operations.open
static int hy_rw_iic_drv_open(struct inode *inode, struct file *filp)
{
	filp->private_data = hy_rw_i2c_dev_tt;
	return 0;
}

// file_operations.release
static int hy_rw_iic_drv_release(struct inode *inode, struct file *filp)
{
	return 0;
}

// file_operations.read
static ssize_t hy_rw_iic_drv_read(struct file *filp, char *buf, size_t count, loff_t *f_pos) {

    struct hy_rw_i2c_dev *dev = filp->private_data;

    if (count < sizeof(dev->rawdata)) {
		printk("[hy4635][Touch] count err.\n");
        return -EBADF;
    }
	if(dev->unread == 0) {
		return 0;
	}
	if(threadRunningStatus == 0) {
		dev->unread = 0;
		return -EFAULT;
	}
	// 休眠设置
	if(wait_event_interruptible(dev->read_queue, dev->unread == 1)) {
		printk("[hy4635][Touch] interruptible err.\n");
        return - ERESTARTSYS;
    }
	if(threadRunningStatus == 0) {
		dev->unread = 0;
		return -EFAULT;
	}
	// 读rawdata数据
    printk("[hy4635][Touch] read count=%d\n", count);
	// 将rawdata数据给界面上的数据结构
    if (copy_to_user(buf, &(dev->rawdata), count)) {
		printk("[hy4635][Touch] copy_to_user err.\n");
        return -EFAULT;
    }
	// 设置已读状态
	dev->unread = 0;

    return count;
}

/*STEP4 扫描ADC*/
static int rawdate_scan_adc(struct hy_rw_i2c_dev* dev) {
	u8 regaddr = 0x02, regvalue = 0xff;
	int err = -1;
	int count = 0;
	if ((err = hy46xx_write_reg(dev->client, regaddr, 0x01)) < 0) {
		printk("%s write regaddr:0x%x failed.\n", __FUNCTION__, regaddr); //写入寄存器出错
		return -1;
	}
	// 读寄存器，如果小于0说明寄存器读取出错了
	while ((err = hy46xx_read_reg(dev->client, regaddr, &regvalue)) >= 0) {
		// ADC返回0时，说明扫描结束，直接break,否则继续读
		if (regvalue != 0x00) {
			gpio_set_value(8 , 1);
			msleep(10);
			gpio_set_value(8 , 0);
			count++;
			if(count > 20) {
				return err;
			}
			continue;
		}
		break;
	}
	return err;
}

/*STEP5,6,7,8 rawdata线程处理*/
static int rawdata_thread_proc(void *arg) {
	struct hy_rw_i2c_dev* dev = (struct hy_rw_i2c_dev*) arg;

	printk(KERN_ALERT"enter rawdata_thread_proc.\n");
	threadRunningStatus = 1;
	while(!kthread_should_stop()) {
		int adc = -1;
		printk(KERN_ALERT"enter rawdata_thread_proc dev->unread:%d.\n", dev->unread);
		if (dev->unread == 1) {
			/*唤醒*/
			wake_up_interruptible(&dev->read_queue);
			msleep(50);
			continue;
		}
		dev->rawdata.mode = dev->user_data.mode;
	//	dev->rawdata.ver = IC_FW_Verson;
		memset(&dev->rawdata.rawdata, 0, sizeof(dev->rawdata.rawdata));
		// STEP4 按4~6步方式 读rawdata 
		adc = rawdate_scan_adc(dev);
		printk(KERN_ALERT"enter rawdata_thread_proc adc:%d\n", adc);
		if (adc >= 0) {
			int ret = 0, i = 0, j = 0;
			char regaddr = 0x10, regvalue[2 * dev->rawdata.tx];
			printk(KERN_ALERT"start read rawdata.\n");
			// STEP5 写寄存器 0x01 值为 TXINDEX 从0x01开始到tx
			for (i = 0; i < dev->rawdata.tx; i++)
			{
				memset(regvalue, 0, 2 * dev->rawdata.rx);
				if ((ret = hy46xx_write_reg(dev->client, 0x01, (i + 1))) >= 0)
				{
					// STEP6 从 TX Index读RAWDATA，首地址0x10，长度30个字节
					if ((ret = hy46xx_i2c_Read(dev->client, &regaddr, 1, regvalue, 2 * dev->rawdata.rx)) >= 0)
					{
						// 将TX INDEX的RAWDATA数据按2位合并一个数
						for (j = 0; j < dev->rawdata.rx; j++)
						{
							dev->rawdata.rawdata[i][j] = ((regvalue[j * 2] << 8) + regvalue[j * 2 + 1]);
							//printk(KERN_ALERT"tx:%d, rx:%d, value:%d", i, j, ((regvalue[j * 2] << 8)  + regvalue[j * 2 + 1]));
						}
					}
					//printk(KERN_ALERT"\n");
				}
				//sprintf(dev->rawdata.rawdata[16], "proc ret:%d\n", ret);
			}

			/*
			dev->rawdata.rawdata[0][0] = 't';
			dev->rawdata.rawdata[0][1] = 'x';
			dev->rawdata.rawdata[0][2] = ':';
			dev->rawdata.rawdata[0][3] = tx;
			dev->rawdata.rawdata[1][0] = 'r';
			dev->rawdata.rawdata[1][1] = 'x';
			dev->rawdata.rawdata[1][2] = ':';
			dev->rawdata.rawdata[1][3] = rx;
			*/

			// 设置成未读状态
			dev->unread = 1;
			/*唤醒*/
			wake_up_interruptible(&dev->read_queue);
			msleep(100);
		}
	}
//	gpio_set_value(8 , 0);
	dev->unread = 1;
	threadRunningStatus = 0;
	wake_up_interruptible(&dev->read_queue);
	return 0;
}

/*STEP1 切换模式*/
static int rawdata_switch_mode(struct i2c_client * client, int mode) {
	u8 regaddr = 0x00, regvalue = 0xff;
	int err = 0;
	//STEP1 a 读寄存器0x00，判断mode
	if ((err = hy46xx_read_reg(client, regaddr, &regvalue)) < 0) {
		printk(KERN_ALERT "%s read mode regaddr: 0x%x failed.\n", __FUNCTION__, regaddr);
		return err;
	}

	if (mode == 1) {
		// 如果是正常模式，就切换成RAWDATA模式，否则不作操作
		if (regvalue == 0x00) {
			//STEP1 b 写寄存器 0x00值为0xC0
			if ((err = hy46xx_write_reg(client, regaddr, 0xC0)) < 0) {
				printk(KERN_ALERT "%s write rawdata mode regaddr: 0x%x value: 0xc0 failed.\n", __FUNCTION__, regaddr);
				return err;
			}
			//STEP1 c 读寄存器 0x00
			if ((err = hy46xx_read_reg(client, regaddr, &regvalue)) < 0) {
				printk(KERN_ALERT "%s read rawdata mode regaddr: 0x%x failed.\n", __FUNCTION__, regaddr);
				return err;
			}
			//STEP1 c 判断RAWDATA模式
			if (regvalue == 0xC0) {
				printk(KERN_ALERT "rawdata mode switch successfully.\n");
			} else {
				printk(KERN_ALERT "rawdata mode switch failed.\n");
			}
		}
	} else {
		// 如果是RAWDATA模式，就切换成正常模式模式，否则不作操作
		if (regvalue == 0xC0) {
			//STEP1 b 写寄存器0x00 值为0x00 正常模式
			if ((err = hy46xx_write_reg(client, regaddr, 0x00)) < 0) {
				printk(KERN_ALERT "write normal mode failed.\n");
				return err;
			}
			printk(KERN_ALERT "rawdata mode switch err1:%d.\n", err);
			//STEP1 c 读寄存器 0x00
			if ((err = hy46xx_read_reg(client, regaddr, &regvalue)) < 0) {
				printk(KERN_ALERT "read normal mode failed.\n");
				return err;
			}
			//STEP1 c 判断正常模式
			if (regvalue == 0x00) {
				printk(KERN_ALERT "normal mode switch successfully.\n");
			} else {
				printk(KERN_ALERT "normal mode switch failed.\n");
			}
		}
	}
	return 0;
}

#if 0
/*STEP2 写CMD*/
static int rawdata_write_cmd(struct hy_rw_i2c_dev* dev) {
	int err = 0;
	char regaddr = 0xDC, regvalue[4];

	//写cmd1, 4个字节
	if ((err = hy46xx_i2c_Write(dev->client, dev->user_data.cmd1, sizeof(dev->user_data.cmd1))) < 0) {
		printk(KERN_ALERT"1st write cmd1 failed.\n");
		return err;
	}
	//写cmd2, 6个字节
	if ((err = hy46xx_i2c_Write(dev->client, dev->user_data.cmd2, sizeof(dev->user_data.cmd2))) < 0) {
		printk(KERN_ALERT"write cmd2 failed.\n");
		return err;
	}
	//再次写cmd1, 4个字节
	if ((err = hy46xx_i2c_Write(dev->client, dev->user_data.cmd1, sizeof(dev->user_data.cmd1))) < 0) {
		printk(KERN_ALERT"2nd write cmd1 failed.\n");
		return err;
	}

	// 读0xDC里的四个字节
	if ((err = hy46xx_i2c_Read(dev->client, &regaddr, 1, regvalue, 4)) < 0) {
		printk(KERN_ALERT"1st read 0xDC failed.\n");
		return err;
	} 
	// 读取值的最后一个字节与cmd2的最后一个字节一样，表示写入成功，否则失败
	if (dev->user_data.cmd2[5] != regvalue[3]) {
		printk(KERN_ALERT"cmd2 is different from the last byte of value.\n");
		return -1;
	}
	//写入cmd3, 4个字节
	if ((err = hy46xx_i2c_Write(dev->client, dev->user_data.cmd3, sizeof(dev->user_data.cmd3))) < 0) {
		printk(KERN_ALERT"1st write cmd3 failed.\n");
		return err;
	}
	//写入cmd4, 6个字节
	if ((err = hy46xx_i2c_Write(dev->client, dev->user_data.cmd4, sizeof(dev->user_data.cmd4))) < 0) {
		printk(KERN_ALERT"write cmd4 failed.\n");
		return err;
	}
	//再次写入cmd3, 4个字节
	if ((err = hy46xx_i2c_Write(dev->client, dev->user_data.cmd3, sizeof(dev->user_data.cmd3))) < 0) {
		printk(KERN_ALERT"2nd write cmd3 failed.\n");
		return err;
	}
	//再次从0xDC中读取4个字节
	memset(regvalue, 0, 4);
	if ((err = hy46xx_i2c_Read(dev->client, &regaddr, 1, regvalue, 4)) < 0) {
		printk(KERN_ALERT"2nd read 0xDC failed.\n");
		return err;
	}
	//读取第二个字节低5位与cmd4的第四个字节低5位一样，表示写入成功 
	if ((regvalue[1] & 0x1f) != (dev->user_data.cmd4[3] & 0x1f)) {
		printk(KERN_ALERT"cmd4 is different from the last byte of value.\n");
		return -1;
	}
	return 0;
}
#endif

/*STEP2/3 读TX RX*/
static int rawdata_read_rx_tx(struct hy_rw_i2c_dev* dev) {
	int err = -1;
	// a 读寄存器 0xAB，获取有效的 TX 数目
	if ((err = hy46xx_read_reg(dev->client, 0xAB, &dev->rawdata.tx)) < 0) {
		printk(KERN_ALERT"read tx failed.\n");
		return err;
	}
	//b 读寄存器 0xAC，获取有效的 RX 数目
	if ((err = hy46xx_read_reg(dev->client, 0xAC, &dev->rawdata.rx)) < 0) {
		printk(KERN_ALERT"read rx failed.\n");
		return err;
	}
	return err;
}

/*STEP3 写寄存器AE,值为0x04*/
static int diffdata_write_ae(struct hy_rw_i2c_dev* dev) {
	int err = -1;
	char regaddr = 0xAE;

	//写AE,值为0x04
	if ((err = hy46xx_write_reg(dev->client, regaddr, 0x04)) < 0) {
		printk(KERN_ALERT"1st write reg 0xae value 0x04 failed.\n");
		return err;
	}
	return err;
}

// file_operations.write
static ssize_t hy_rw_iic_drv_write(struct file *filp, const char *buf, size_t count, loff_t *f_pos) {
    struct hy_rw_i2c_dev* dev = filp->private_data;
	ssize_t err = 0;
	int cnt = 0;
	struct hy46xx_ts_data *ts;
    if (count != sizeof(dev->user_data)) {
        printk(KERN_ALERT" count error\n");
		err = -EBADF;
        goto out;
    }

	//读取界面传来的数据
    if(copy_from_user(&(dev->user_data), buf, count)) {
        printk(KERN_ALERT"copy data from user space, failed\n");
		err = -EFAULT;
        goto out;
    }

	printk(KERN_ALERT"start create/stop kthread.\n");
	// 1开启模式，0关闭模式
	if (dev->user_data.mode == 1) {
		if (thread != NULL) {
			return 0;
		}
//		gpio_set_value(8 , 0);
		ts = i2c_get_clientdata(dev->client);
		gpio_direction_output(desc_to_gpio(ts->gpiod_rst), 0);
		msleep(90);
		hy46xx_reset_for_mode_switch(i2c_get_clientdata(dev->client));
		thread = kthread_create(rawdata_thread_proc, dev, "thread %d", 0);
		if (IS_ERR(thread)) {
             printk(KERN_ALERT"create thread err.\n");
			 err = PTR_ERR(thread);
			 goto out;
        }
		//STEP1 切换rawdata模式，最多切换3次
		while(((err = rawdata_switch_mode(dev->client, 1)) < 0) && cnt < 3) {
			cnt++;
		}
		printk(KERN_ALERT"start rawdata_switch_mode err:%d, cnt:%d.\n", err, cnt);
		if (cnt >= 3) {
			goto out;
		}
		msleep(100);
		cnt = 0;
		//STEP2 写入CMD操作
		/*if ((err = rawdata_write_cmd(dev)) < 0) {
			goto out;
		}
		//STEP3 读TX与RX*/
		//STEP2 读TX与RX
		if ((err = rawdata_read_rx_tx(dev)) < 0) {
			goto out;
		}
		msleep(20);
		printk(KERN_ALERT"start rawdata_read_rx_tx.\n");
		//STEP3 写寄存器AE,值为0X04
		if ((err = diffdata_write_ae(dev)) < 0) {
			goto out;
		}
		msleep(20);
		printk(KERN_ALERT"start diffdata_write_ae err:%d.\n", err);
		wake_up_process(thread);
		dev->unread = 0;
		err = sizeof(dev->user_data);
	} else if(dev->user_data.mode == 0) {
		
		printk(KERN_ALERT" start out thread.\n");
		if (thread) {
			kthread_stop(thread);
			thread = NULL;
		}
		printk(KERN_ALERT" end out thread.\n");
		dev->unread = 1;
		wake_up_interruptible(&dev->read_queue);
		hy46xx_reset_for_mode_switch(i2c_get_clientdata(dev->client));
		// 切换正常模式，最多切换3次
		while(((err = rawdata_switch_mode(dev->client, 0)) < 0) && cnt < 3) {
			cnt++;
		}
		printk(KERN_ALERT"start close rawdata_switch_mode err:%d.\n", err);
		if (cnt >= 3) {
			goto out;
		}
		printk(KERN_ALERT"end close rawdata_switch_mode cnt:%d.\n", cnt);
		cnt = 0;
		return err;
		goto out;
	} else {
		err = -ESRCH;
		goto out;
	}

	return err;

out:
#if 0
	dev->rawdata.mode = dev->user_data.mode;
	// 设置成未读状态
	dev->unread = 1;
	/*唤醒*/
	wake_up_interruptible(&dev->read_queue);
#endif
	gpio_set_value(8 , 1);
	printk(KERN_ALERT" start out thread.\n");
	if (thread) {
		kthread_stop(thread);
		thread = NULL;
	}
	printk(KERN_ALERT" end out thread.\n");
	dev->unread = 1;
	wake_up_interruptible(&dev->read_queue);
//	hy46xx_reset_for_mode_switch(i2c_get_clientdata(dev->client));
//	hy46xx_reset_tp(i2c_get_clientdata(dev->client));
    return 0;
}

long int hy_rw_iic_drv_ioctl1(struct file *filp, unsigned int cmd, unsigned long __user arg)	
{
	int *fw = (int *)arg;
	int ret = copy_to_user(fw , &IC_FW_Verson , 1);
	printk("fw version %x\n" , ret);
	return (long int)IC_FW_Verson;	
}

static const struct file_operations hy_rw_iic_drv_fops = {
	.owner			= THIS_MODULE,
	.open			= hy_rw_iic_drv_open,
	.release		= hy_rw_iic_drv_release,
    .read           = hy_rw_iic_drv_read,
    .write          = hy_rw_iic_drv_write,
	.unlocked_ioctl 	= hy_rw_iic_drv_ioctl1,
};
/************************************************************/
static void hy_rw_iic_drv_setup_cdev(struct hy_rw_i2c_dev *dev, int index)
{
	int err, devno = MKDEV(hy_rw_iic_drv_major, index);

	cdev_init(&dev->cdev, &hy_rw_iic_drv_fops);
	dev->cdev.owner = THIS_MODULE;
	dev->cdev.ops = &hy_rw_iic_drv_fops;

	/*初始化等待队列头*/
	init_waitqueue_head(&dev->read_queue);

	err = cdev_add(&dev->cdev, devno, 1);
	if (err)
		printk(KERN_NOTICE "Error %d adding LED%d", err, index);
}
/************************************************************/
static int hy_rw_iic_drv_myinitdev(struct i2c_client *client)
{
	int err = 0;
	struct device* temp = NULL;

	dev_t devno = MKDEV(hy_rw_iic_drv_major, 0);

	if (hy_rw_iic_drv_major)
		err = register_chrdev_region(devno, 1, HY_RW_IIC_DRV);
	else {
		err = alloc_chrdev_region(&devno, 0, 1, HY_RW_IIC_DRV);
		hy_rw_iic_drv_major = MAJOR(devno);
	}
	if (err < 0) {
		dev_err(&client->dev, "%s:hy_rw_iic_drv failed  error code=%d---\n",
				__func__, err);
		return err;
	}

	hy_rw_i2c_dev_tt = kmalloc(sizeof(struct hy_rw_i2c_dev), GFP_KERNEL);
	memset(hy_rw_i2c_dev_tt, 0, sizeof(struct hy_rw_i2c_dev));

	if (!hy_rw_i2c_dev_tt){
		err = -ENOMEM;
		unregister_chrdev_region(devno, 1);
		dev_err(&client->dev, "%s:hy_rw_iic_drv failed\n",__func__);
		return err;
	}
	hy_rw_i2c_dev_tt->client = client;
	
	mutex_init(&hy_rw_i2c_dev_tt->hy_rw_i2c_mutex);
	hy_rw_iic_drv_setup_cdev(hy_rw_i2c_dev_tt, 0); 

	hys_class = class_create(THIS_MODULE, "hys_class");
	if (IS_ERR(hys_class)) {
		dev_err(&client->dev, "%s:failed in creating class.\n",
				__func__);
		return -1; 
	} 
	temp = device_create(hys_class, NULL, MKDEV(hy_rw_iic_drv_major, 0), 
			NULL, HY_RW_IIC_DRV);
	if(IS_ERR(temp)) {
        err = PTR_ERR(temp);
        printk(KERN_ALERT"Failed to create tpd debug device.\n");
        goto destroy_class;
    }

	dev_set_drvdata(temp, hy_rw_i2c_dev_tt);

	return 0;
destroy_class:
    class_destroy(hys_class);
	return err;
}
/************************************************************/
int hy_rw_iic_drv_init(struct i2c_client *client)
{
	return hy_rw_iic_drv_myinitdev(client);
}
/************************************************************/
void  hy_rw_iic_drv_exit(void)
{
	if (thread) {
		wake_up_interruptible(&hy_rw_i2c_dev_tt->read_queue);
		hy_rw_i2c_dev_tt->unread = 0;
		kthread_stop(thread);
		thread = NULL;
	}
	device_destroy(hys_class, MKDEV(hy_rw_iic_drv_major, 0)); 
	class_destroy(hys_class); 
	cdev_del(&hy_rw_i2c_dev_tt->cdev);
	kfree(hy_rw_i2c_dev_tt);
	unregister_chrdev_region(MKDEV(hy_rw_iic_drv_major, 0), 1); 
}
#endif
/**************************************************************/
static int hy46xx_read_Touchdata(struct hy46xx_ts_data *data)
{
	struct ts_event *event = &data->event;
	unsigned char  buf[POINT_READ_BUF] = { 0 };
	int ret = -1;
	int i = 0;
	unsigned char pointid = HY_MAX_ID;

	for(i = 0; i < POINT_READ_BUF; i++)
	{
		buf[i] = 0xff;
	}
	memset(event, 0, sizeof(struct ts_event));
	buf[0] = 0;
	ret = hy46xx_i2c_Read(data->client, buf, 1, buf, POINT_READ_BUF);
	if (ret < 0) {
		dev_err(&data->client->dev, "%s read touchdata failed.\n",
			__func__);
		return ret;
	}
	#if 0
	for(i = 0; i < 9; i++)
	{
		pr_info("buf[%d]= 0x%x  ", i,buf[i]);
	}
	#endif
	event->touch_point = 0;
	for (i = 0; i < CFG_MAX_TOUCH_POINTS; i++) {
		pointid = (buf[HY_TOUCH_ID_POS + HY_TOUCH_STEP * i]) >> 4;
		if (pointid >= HY_MAX_ID)
			break;
		else{
			event->touch_point++;
			event->au16_x[i] =(unsigned short) (buf[HY_TOUCH_X_H_POS + HY_TOUCH_STEP * i] & 0x0F) << 8 | (s16) buf[HY_TOUCH_X_L_POS + HY_TOUCH_STEP * i];
//			if(event->au16_x[i] <= 42) {
//				event->au16_x[i] = 0;
//			}
//			else if(event->au16_x[i] >= (1024+42)){
//				event->au16_x[i] = 1024;
//			}
//			else {
//				event->au16_x[i] -= 42;
//			}
			event->au16_y[i] =(unsigned short) (buf[HY_TOUCH_Y_H_POS + HY_TOUCH_STEP * i] & 0x0F) <<8 | (s16) buf[HY_TOUCH_Y_L_POS + HY_TOUCH_STEP * i];
//			if(event->au16_y[i] <= 5) {
//				event->au16_y[i] = 0;
//			}
//			else {
//				event->au16_y[i] -= 5;
//			}
			event->au8_touch_event[i] =buf[HY_TOUCH_EVENT_POS + HY_TOUCH_STEP * i] >> 6;
			event->au8_finger_id[i] =(buf[HY_TOUCH_ID_POS + HY_TOUCH_STEP * i]) >> 4;
			break;
		}
		#if 0
		pr_info("id=%d event=%d x=%d y=%d\n", event->au8_finger_id[i],event->au8_touch_event[i], event->au16_x[i], event->au16_y[i]);
		#endif
	}

	event->pressure = 0x05;

	return 0;
}
/**************************************************************/
static void hy46xx_report_value(struct hy46xx_ts_data *data)
{
	struct ts_event *event = &data->event;
	int i;
	int uppoint = 0;

	for (i = 0; i < event->touch_point; i++)
	{
		if((event->au16_x[i] < RESOLUTION_X) && (event->au16_y[i] < RESOLUTION_Y))
		{
			input_mt_slot(data->input_dev, event->au8_finger_id[i]);			
			if (event->au8_touch_event[i]== 0 || event->au8_touch_event[i] == 2)
			{								
				input_mt_report_slot_state(data->input_dev, MT_TOOL_FINGER,true);
				input_report_abs(data->input_dev, ABS_MT_TOUCH_MAJOR,event->pressure);
				input_report_abs(data->input_dev, ABS_MT_POSITION_X,event->au16_x[i]);
				input_report_abs(data->input_dev, ABS_MT_POSITION_Y,event->au16_y[i]);		
				printk("x:%d , y:%d\n" , event->au16_x[i] , event->au16_y[i]);
			}
			else
			{
				uppoint++;
				input_mt_report_slot_state(data->input_dev, MT_TOOL_FINGER,false);
			}
			
		}
		else
		{
			input_mt_slot(data->input_dev, event->au8_finger_id[i]);
			input_mt_report_slot_state(data->input_dev, MT_TOOL_FINGER,false);
		}
		
#ifdef LINUX_OS
		input_mt_sync(data->input_dev);
#endif
	}
	if(event->touch_point == uppoint)
		input_report_key(data->input_dev, BTN_TOUCH, 0);
	else
		input_report_key(data->input_dev, BTN_TOUCH, 1);
	input_sync(data->input_dev);

}
/**************************************************************/
static irqreturn_t hy46xx_ts_interrupt(int irq, void *dev_id)
{
	unsigned char uc_reg_addr; 
	struct hy46xx_ts_data *hy46xx_ts = dev_id;
	//int ret = 0;
//	disable_irq_nosync(hy46xx_ts->irq);
	hy46xx_read_Touchdata(hy46xx_ts);
	hy46xx_report_value(hy46xx_ts);

	if(IC_FW_Verson == 0xa6) 
	{
		uc_reg_addr = HY46XX_REG_FW_VER;
        hy46xx_i2c_Read(hy46xx_ts->client, &uc_reg_addr, 1, &IC_FW_Verson, 1);
	}

//	enable_irq(hy46xx_ts->irq);
	return IRQ_HANDLED;
}
/**********************************************************************************/
static int hy46xx_ts_probe(struct i2c_client *client,const struct i2c_device_id *id)
{
//	struct hy46xx_platform_data *pdata = (struct hy46xx_platform_data *)client->dev.platform_data;
	struct hy46xx_ts_data *hy46xx_ts;
	struct input_dev *input_dev;
	int err = 0;
//	unsigned char IC_FW_Verson;
//	unsigned char I_File_Version;
	unsigned char uc_reg_addr;
	u32 gpio_rst;
    u32 gpio_int;
	
	printk("%s\n" , __FUNCTION__);
	if (!i2c_check_functionality(client->adapter, I2C_FUNC_I2C)) {
		err = -ENODEV;
		goto exit_check_functionality_failed;
	}

	hy46xx_ts = kzalloc(sizeof(struct hy46xx_ts_data), GFP_KERNEL);

	if (!hy46xx_ts) {
		err = -ENOMEM;
		goto exit_alloc_data_failed;
	}

	if(0 != of_property_read_u32(client->dev.of_node, GOODIX_GPIO_RST_NAME, &gpio_rst))
        return -EINVAL;
    
    if(0 != of_property_read_u32(client->dev.of_node, GOODIX_GPIO_INT_NAME, &gpio_int))
        return -EINVAL;
	
    hy46xx_ts->gpiod_int = gpio_to_desc(gpio_int);
	hy46xx_ts->gpiod_rst = gpio_to_desc(gpio_rst);
	printk("rst %d , int %d\n" , gpio_rst , gpio_int);
	hy46xx_ts->client = client;
	i2c_set_clientdata(client, hy46xx_ts);
	
	hy46xx_ts->client->irq = of_irq_get_byname(client->dev.of_node, GOODIX_GPIO_INT_NAME);
	printk("irq %d\n" , hy46xx_ts->client->irq);
//	pdata->x_max = 1024;
//	pdata->y_max = 600;
	
	hy46xx_ts->irq = hy46xx_ts->client->irq;	
	
//	hy46xx_ts->pdata = pdata;
	hy46xx_ts->x_max = 1023;
	hy46xx_ts->y_max = 599;
//	hy46xx_ts->pdata->reset = desc_to_gpio(hy46xx_ts->gpiod_rst);
//	hy46xx_ts->pdata->irq = hy46xx_ts->irq;
//	hy46xx_ts->irq_flags = IRQ_TYPE_EDGE_FALLING | IRQF_ONESHOT;
	//pr_info("irq = %d\n", client->irq);
	//	  devm_request_threaded_irq(&ts->client->dev, ts->client->irq,NULL, goodix_ts_irq_handler,ts->irq_flags, ts->client->name, ts);
	
//	disable_irq(client->irq);
	printk("disable irq\n");
	input_dev = input_allocate_device();
	if (!input_dev) {
		err = -ENOMEM;
		dev_err(&client->dev, "failed to allocate input device\n");
		goto exit_input_dev_alloc_failed;
	}

	hy46xx_ts->input_dev = input_dev;

	__set_bit(EV_ABS, input_dev->evbit);
	__set_bit(EV_KEY, input_dev->evbit);
	__set_bit(BTN_TOUCH, input_dev->keybit);
	
	input_mt_init_slots(input_dev, CFG_MAX_TOUCH_POINTS,INPUT_MT_DIRECT | INPUT_MT_DROP_UNUSED);
	input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0, 0XFF, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_X,  0, (RESOLUTION_X-1), 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y,  0, (RESOLUTION_Y-1), 0, 0);

	input_dev->name = HY46XX_NAME;
	printk("tp name %s\n" , HY46XX_NAME);
	input_set_abs_params(input_dev, ABS_MT_TOUCH_MAJOR, 0, 0XFF, 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_X,  0, (RESOLUTION_X-1), 0, 0);
	input_set_abs_params(input_dev, ABS_MT_POSITION_Y,  0, (RESOLUTION_Y-1), 0, 0);

	input_dev->name = HY46XX_NAME;
	printk("tp name %s\n" , HY46XX_NAME);
	err = input_register_device(input_dev);
	if (err) {
		dev_err(&client->dev,
			"hy46xx_ts_probe: failed to register input device: %s\n",
			dev_name(&client->dev));
		goto exit_input_register_device_failed;
	}	

	hy46xx_reset_tp(hy46xx_ts);
//	msleep(2000);
	
	err = devm_request_threaded_irq(&client->dev , client->irq, NULL, hy46xx_ts_interrupt, IRQF_TRIGGER_FALLING | IRQF_ONESHOT ,client->dev.driver->name,hy46xx_ts);
	if (err < 0) {
		dev_err(&client->dev, "hy46xx_probe: request irq failed\n");
		goto exit_irq_request_failed;
	}
	/*get firmware version */
	uc_reg_addr = HY46XX_REG_FW_VER;
	hy46xx_i2c_Read(client, &uc_reg_addr, 1, &IC_FW_Verson, 1);
//	hy_rw_iic_drv_init(client);
	printk( "[HYS] Firmware version = 0x%x\n", IC_FW_Verson);
#ifdef HYS_READ_RAWDATA
	hy_rw_iic_drv_init(client);
#endif
//	enable_irq(client->irq);
	return 0;

exit_input_register_device_failed:
	input_free_device(input_dev);

exit_input_dev_alloc_failed:
	free_irq(client->irq, hy46xx_ts);

//exit_init_gpio:
//	hys_un_init_gpio_hw(hy46xx_ts);

exit_irq_request_failed:
	i2c_set_clientdata(client, NULL);
	kfree(hy46xx_ts);

exit_alloc_data_failed:
exit_check_functionality_failed:
	return err;
}
static const struct i2c_device_id hy46xx_ts_id[] = {
	{"GDIX1001:00", 0},
	{}
};
static const struct of_device_id hy46xx_dt_match[] = {
	{.compatible = "goodix,gt911"},
	{},
};
MODULE_DEVICE_TABLE(of, hy46xx_dt_match);
//MODULE_DEVICE_TABLE(i2c, hy46xx_ts_id);

/************************************************************/
static int  hy46xx_ts_remove(struct i2c_client *client)
{
	struct hy46xx_ts_data *hy46xx_ts;
	hy46xx_ts = i2c_get_clientdata(client);
	input_unregister_device(hy46xx_ts->input_dev);
	#ifdef HYS_READ_RAWDATA
		hy_rw_iic_drv_exit();
	#endif
	#ifdef HYS_APK_DEBUG
		hy46xx_remove_sysfs(client);
		hy_rw_iic_drv_exit();
		hy46xx_release_apk_debug_channel();
	#endif

	free_irq(client->irq, hy46xx_ts);

	kfree(hy46xx_ts);
	i2c_set_clientdata(client, NULL);
	return 0;
}
static struct i2c_driver hy46xx_ts_driver = {
	.probe = hy46xx_ts_probe,
	.remove = hy46xx_ts_remove,
	.id_table = hy46xx_ts_id,
//	.suspend = hy46xx_ts_suspend,
//	.resume = hy46xx_ts_resume,
	.driver = {
		   .name = HY46XX_NAME,
		   .owner = THIS_MODULE,
		   .of_match_table = hy46xx_dt_match,
		   },
};
/************************************************************/
static int __init hy46xx_ts_init(void)
{
	int ret;
	ret = i2c_add_driver(&hy46xx_ts_driver);
	if (ret) {
		printk(KERN_WARNING "Adding hy46xx driver failed "
		       "(errno = %d)\n", ret);
	} else {
		pr_info("Successfully added driver %s\n",
			hy46xx_ts_driver.driver.name);
	}
	return ret;
}
/************************************************************/
static void __exit hy46xx_ts_exit(void)
{
	i2c_del_driver(&hy46xx_ts_driver);
}

module_init(hy46xx_ts_init);
module_exit(hy46xx_ts_exit);

MODULE_AUTHOR("<tp term>");
MODULE_DESCRIPTION("HYCON hy46xx TouchScreen driver");
MODULE_LICENSE("GPL");
