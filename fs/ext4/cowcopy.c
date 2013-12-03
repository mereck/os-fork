#include <linux/fs.h>
#include <linux/string.h>
#include <linux/namei.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>

asmlinkage int sys_ext4_cowcopy(const char __user *src, const char __user *dest){

	struct path path;
	char buf[100];
	int ret;
	struct dentry* d;
	const char *file_system_type;

	printk ("**COWCOPY TRIGGERED**\n");
	ret = copy_from_user(&buf, src, sizeof(buf));
	printk ("**COWCOPY buffer: %s, ret: %d \n",buf,ret);

	kern_path(buf,LOOKUP_FOLLOW,&path);	

	d = path.dentry;
	//printk("**COWCOPY path: %s\n**",path);

	if (d == NULL){
		printk("**COWCOPY path.dentry is null**\n");
		return 0;
	}

	if (d->d_sb == NULL){
		printk("**COWCOPY d->d_sb is null**\n");
		return 0;
	}

	if ((d->d_sb)->s_type == NULL){
		printk("**COWCOPY source file is not found. d->d_sb->s_type is null**\n");
		return 0;
	}

	if (((d->d_sb)->s_type)->name == NULL){
		printk("**COWCOPY d->d_sb->s_type->name is null**\n");
		return 0;
	}

	file_system_type = ((d->d_sb)->s_type)->name;
	printk("**COWCOPY: file system type name:%s**\n",file_system_type);

	ret = strncmp(file_system_type,"ext4",strlen(buf));

	if (ret != 0){
		printk("**COWCOPY file system type doesn't match ext4**\n");
		return -EOPNOTSUPP;
	} else {
			printk("**COWCOPY file system type matches ext4**\n");
	}
		
	

	return 0;


}
