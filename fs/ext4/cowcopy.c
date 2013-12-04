#include <linux/fs.h>
#include <linux/string.h>
#include <linux/namei.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>

int checkFsType(struct dentry* d, char *buf){
	const char *file_system_type;
	int ret;
	if (d == NULL){
		printk("**COWCOPY path.dentry is null**\n");
		return 1;
	}

	if (d->d_sb == NULL){
		printk("**COWCOPY d->d_sb is null**\n");
		return 1;
	}

	if ((d->d_sb)->s_type == NULL){
		printk("**COWCOPY source file is not found. d->d_sb->s_type is null**\n");
		return 1;
	}

	if (((d->d_sb)->s_type)->name == NULL){
		printk("**COWCOPY d->d_sb->s_type->name is null**\n");
		return 1;
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

// if inode is a directory, return 2, otherwise return 0. return 1 on error
int checkInodeType(struct inode* i){
	if (i == NULL){
		printk("**COWCOPY source file inode is null. d.d_inode is null.**\n");
		return 1;
	}
	//if (i->i_mode == NULL){
	//	printk("**COWCOPY source file inode.i_mode is null.**\n");
	//	return 1;
	//}

	if(S_ISDIR(i->i_mode)==0){
		printk("**COWCOPY source file is a directory!**\n");
		return 2;
	}

	return 0;
}

int checkDevices(char *s, char *d){
	return 0;
}

asmlinkage int sys_ext4_cowcopy(const char __user *src, const char __user *dest){

	struct path path;
	int ret;
	char buf[100]; //TODO: we need to get this to work with paths that are larger than buffer size
	char *ksource;
	char *kdest;

	ret = copy_from_user(&buf, src, sizeof(buf));
	ksource = buf;
	printk ("**COWCOPY src: %s\n",ksource);

	kern_path(ksource,LOOKUP_FOLLOW,&path);	

	if(checkFsType(path.dentry,ksource) == 1)
		return 1;

	ret = checkInodeType(path.dentry->d_inode);

	if (ret == 2) //src is a directory
		return -EPERM;
	else if (ret == 1)
		return 1;

	ret = copy_from_user(&buf, dest, sizeof(buf));
	kdest = buf;
	printk ("**COWCOPY dest: %s**\n",kdest);

	if(checkDevices(ksource,kdest) == 1)
		return 1;

	return 0;


}
