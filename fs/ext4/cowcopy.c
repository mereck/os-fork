#include <linux/fs.h>
#include <linux/string.h>
#include <linux/namei.h>


asmlinkage int sys_ext4_cowcopy(const char __user *src, const char __user *dest){

	char *filename;
	char *last_slash;
	struct path path;

	last_slash = strrchr(src,'/');
	if (last_slash != NULL){
		filename = last_slash+1;
		last_slash = '\0';
	}else {
		return 0;
	}

	printk("filename:%s\n",filename);
	printk("path:%s\n",src);
	kern_path(filename,LOOKUP_FOLLOW,&path);
	

	return 0;


}
