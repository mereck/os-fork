#include <linux/fs.h>
#include <linux/string.h>
#include <linux/namei.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>
#include <linux/mutex.h>
#include <linux/xattr.h>

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
	//if (i->i_mode == NULL){
	//	printk("**COWCOPY source file inode.i_mode is null.**\n");
	//	return 1;
	//}

	if(S_ISDIR(i->i_mode)==1){
		printk("**COWCOPY source file is a directory!**\n");
		return 2;
	}

	return 0;
}

int checkDevices(char *s, char *d){
	return 0;
}

// locks parent dentry, taken from ecryptfs/inode.c
/*static struct dentry *lock_parent(struct dentry *dentry){
	struct dentry *dir;
	dir = dget_parent(dentry);
	mutex_lock_nested(&(dir->d_inode->i_mutex), I_MUTEX_PARENT);
	return dir;
}
*/
int createShallowCopy(struct dentry *s, struct dentry *d){
	
	int ret;
	struct inode * i;
	struct dentry * p;
	p = d->d_parent;
	i = p->d_inode;
	printk ("**COWCOPY csc dst parent name:  %s\n",p->d_iname);
	printk ("**COWCOPY csc src parent name:  %s\n",s->d_parent->d_iname);
	//dget(s);
	//printk ("**COWCOPY csc dget(d) \n");
	//dget(d);
	//printk ("**COWCOPY csc lock_parent(d) \n");
	//dir = lock_parent(d);
	//printk ("**COWCOPY csc vfs_link() \n");
	ret = vfs_link(s,i,d); //create hardlink
	
	if (ret == 0){
		printk ("**COWCOPY csc about to setxattr\n");
		ret = vfs_setxattr(s,"cow","1",sizeof("cow"),XATTR_CREATE);
	}




	/*printk ("**COWCOPY csc if(ret || !s-<d_inode)\n");
	if (ret || !s->d_inode){
		printk ("**COWCOPY csc mutex_unlock()\n");
		mutex_unlock(&dir->d_inode->i_mutex);
		printk ("**COWCOPY csc dput(dir)\n");
		dput(dir);
		printk ("**COWCOPY csc dput(s) \n");
		dput(s);
		printk ("**COWCOPY csc dput(d)\n");
		dput(d);
	}*/
	//increment link count for the inode
	//printk ("**COWCOPY csc set_nlink: \n");
	//set_nlink(s->d_inode,s->d_inode->i_nlink+1);
	return 0;
}

asmlinkage int sys_ext4_cowcopy(const char __user *src, const char __user *dest){

	struct path spath, dpath;
	int ret;
	char ksource[100]; //TODO: we need to get this to work with paths that are larger than buffer size
	char kdest[100];
	
	// process source path
	ret = copy_from_user(&ksource, src, sizeof(ksource));
	printk ("**COWCOPY src: %s\n",ksource);

	ret = kern_path(ksource,LOOKUP_FOLLOW,&spath);	

	if(ret < 0)
		return 1;

	if(checkFsType(spath.dentry,ksource) == 1)
		return 1;

	ret = checkInodeType(spath.dentry->d_inode);

	if (ret == 2) //src is a directory
		return -EPERM;
	else if (ret == 1)
		return 1;

	// proces destination path
	printk ("**COWCOPY about to copy dest into buffer**\n");
	ret = copy_from_user(&kdest, dest, sizeof(kdest));
	printk ("**COWCOPY dest: %s**\n",kdest);


	ret = kern_path(kdest,LOOKUP_PARENT,&dpath);

	//if(checkDevices(ksource,kdest) == 1)
	//	return 1;

	if(createShallowCopy(spath.dentry,dpath.dentry) == 1)
		return 1;

	return 0;


}
