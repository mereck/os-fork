#include <linux/fs.h>
#include <linux/string.h>
#include <linux/namei.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/xattr.h>
#include <linux/mount.h>
#include <linux/security.h>

int checkFsType(struct dentry* d, char *buf){
	const char *file_system_type;
	int ret;
	printk("**COWCOPY checkFsType buf: %s **\n",buf);
	if (d == NULL){
		printk("**COWCOPY %s path.dentry is null**\n",buf);
		return 1;
	} else {
		printk("**COWCOPY %s path.dentry is not null**\n",buf);
	}

	if (d->d_sb == NULL){
		printk("**COWCOPY %s d->d_sb is null**\n",buf);
		return 1;
	}

	if ((d->d_sb)->s_type == NULL){
		printk("**COWCOPY %s is not found. d->d_sb->s_type is null**\n",buf);
		return 1;
	}

	if (((d->d_sb)->s_type)->name == NULL){
		printk("**COWCOPY %s d->d_sb->s_type->name is null**\n",buf);
		return 1;
	}

	file_system_type = ((d->d_sb)->s_type)->name;
	printk("**COWCOPY: %s fs type name:%s**\n",buf,file_system_type);

	ret = strncmp(file_system_type,"ext4",strlen(buf));

	if (ret != 0){
		printk("**COWCOPY %s file system type doesn't match ext4**\n",buf);
		return -EOPNOTSUPP;
	} else {
			printk("**COWCOPY %s file system type matches ext4**\n",buf);
	}

	return 0;
}

// if inode is a directory, return 2, otherwise return 0. return 1 on error
int checkInodeType(struct inode* i){
	if (i == NULL){
		printk("**COWCOPY source file inode is null. d.d_inode is null.**\n");
		return 1;
	}

	if(S_ISDIR(i->i_mode)==1){
		printk("**COWCOPY source file is a directory!**\n");
		return 2;
	}

	return 0;
}


asmlinkage int sys_ext4_cowcopy(const char __user *src, const char __user *dest){

	struct path spath, dpath;
	struct dentry * destdentry;
	int ret;
	char ksource[400];
	
	// process source path
	ret = copy_from_user(ksource, src, sizeof(ksource));
		
	printk ("**COWCOPY src: %s\n",ksource);

	ret = kern_path(ksource,LOOKUP_FOLLOW,&spath);	

	if(ret < 0)
		return ret;

	if(checkFsType(spath.dentry,ksource) == 1)
		return ret;

	ret = checkInodeType(spath.dentry->d_inode);

	if (ret == 2) //src is a directory
		return -EPERM;
	else if (ret == 1)
		return ret;

	destdentry = user_path_create(-1,dest,&dpath,0);


	if (IS_ERR(destdentry))
		goto out;

	printk ("**COWCOPY: upc returned dentry:%s\n**",destdentry->d_iname);

	ret = -EXDEV;
	if(spath.mnt != dpath.mnt)
		goto out;

	ret = mnt_want_write(dpath.mnt);
	if(ret)
		goto out;

	ret = security_path_link(spath.dentry,&dpath,destdentry);
	
	if(ret)
		goto out_drop_write;

	ret = vfs_link(spath.dentry,dpath.dentry->d_inode, destdentry);
	printk ("**COWCOPY vfs_link returned %d\n**",ret);
	printk ("**COWCOPY csc about to setxattr\n");
	ret = vfs_setxattr(spath.dentry,"cow","1",sizeof("cow"),XATTR_CREATE);
	printk ("**COWCOPY vfs_setxattr returned %d\n**",ret);



out_drop_write:
	mnt_drop_write(dpath.mnt);
out:
	dput(destdentry);
	mutex_unlock(&dpath.dentry->d_inode->i_mutex);
	path_put(&dpath);
	path_put(&spath);
	return ret;


}


