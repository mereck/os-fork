#include <linux/fs.h>
#include <linux/string.h>
#include <linux/namei.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/security.h>


#include <linux/time.h>
#include <linux/fs.h>
#include <linux/jbd2.h>
#include <linux/mount.h>
#include <linux/path.h>
#include <linux/quotaops.h>
#include "ext4.h"
#include "ext4_jbd2.h"
#include "xattr.h"
#include "acl.h"



asmlinkage int sys_ext4_cowcopy(const char __user *src, const char __user *dest){

	struct path spath, dpath;
	struct dentry * destdentry;
	int ret;
	const char *file_system_type;	
	char *buf;

	ret = user_path_at(AT_FDCWD, src, LOOKUP_FOLLOW, &spath);
	printk("**COWCOPY: user_path_at src returned %d**\n",ret);
	if(ret || spath.dentry->d_inode == NULL)
		return ret;

	file_system_type = ((spath.dentry->d_sb)->s_type)->name;
	buf = spath.dentry->d_iname;
	printk("**COWCOPY: %s fs type name:%s**\n",buf,file_system_type);
	ret = strncmp(file_system_type,"ext4",strlen(file_system_type));

	if (ret != 0){
		printk("**COWCOPY %s file system type doesn't match ext4**\n",buf);
		ret = -EOPNOTSUPP;
		goto out;
	} else {
			printk("**COWCOPY %s file system type matches ext4**\n",buf);
	}

	if (spath.dentry->d_inode == NULL){
		printk("**COWCOPY source file inode is null. d.d_inode is null.**\n");
		goto out;
	}

	if(S_ISDIR(spath.dentry->d_inode->i_mode)==1){
		printk("**COWCOPY source file is a directory!**\n");
		ret =-EPERM;
		goto out;
	}

	// create shallow copy
	destdentry = user_path_create(AT_FDCWD,dest,&dpath,0);
	printk ("**COWCOPY: upc returned dentry:%s**\n",destdentry->d_iname);

	if (IS_ERR(destdentry))
		goto out;


	ret = -EXDEV;
	if(spath.mnt != dpath.mnt){
		printk ("**COWCOPY devices don't match \n**");
		goto out_dput;
	}

//	ret = mnt_want_write(dpath.mnt);
//	printk ("**COWCOPY mnt_want_write returned %d\n**",ret);
	
//	if(ret)
//		goto out_dput;

//	ret = security_path_link(spath.dentry,&dpath,destdentry);
//	printk ("**COWCOPY security_path_link returned %d\n**",ret);
	
	
//	if(ret)
//		goto out_drop_write;

	ret = vfs_link(spath.dentry,dpath.dentry->d_inode, destdentry);
	printk ("**COWCOPY vfs_link returned %d\n**",ret);
	
	printk ("**COWCOPY csc about to setxattr\n");
	ret = ext4_xattr_set(spath.dentry->d_inode,EXT4_INODE_EXTENTS,"cow","1",sizeof("1"),0);
	printk ("**COWCOPY vfs_setxattr on dst returned %d\n**",ret);

//	ret = ext4_xattr_get(spath.dentry->d_inode,EXT4_XATTR_INDEX_TRUSTED,"cow",&buf,sizeof("cow"));
//	printk ("**COWCOPY ext4_get_xattr on src returned %d buf is %s\n**",ret,buf);



//out_drop_write:
//	mnt_drop_write(dpath.mnt);
out_dput:
	dput(destdentry);
	mutex_unlock(&dpath.dentry->d_inode->i_mutex);
	path_put(&dpath);
out:
	path_put(&spath);

	return ret;
}


