#include <linux/fs.h>
#include <linux/string.h>
#include <linux/namei.h>
#include <asm/uaccess.h>
#include <linux/kernel.h>
#include <linux/slab.h>

asmlinkage int sys_ext4_cowcopy(const char __user *src, const char __user *dest){

//	char *filename;
//	char *last_slash;
	//struct path path;

	char buf[100];
	int ret;

	//buf = kmalloc(4*sizeof(char),GFP_KERNEL);
	printk ("**COWCOPY TRIGGERED**\n");
	ret = copy_from_user(&buf, src, sizeof(buf));
	printk ("**COWCOPY buffer: %s, ret: %d \n",buf,ret);
	//kfree(buf);


//	last_slash = strrchr(src,'/');
//	if (last_slash != NULL){
//		filename = last_slash+1;
//		last_slash = '\0';
//	}else {
//		return 0;
//	}

//	printk("**COWCOPY filename:%s**\n",filename);
	//printk("path:%s\n",src);
	//kern_path(filename,LOOKUP_FOLLOW,&path);	

	return 0;


}
