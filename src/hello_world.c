#define pr_fmt(fmt) KBUILD_MODNAME ":" fmt

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/stat.h>
#include <linux/init.h>


static unsigned char idx = 0;
static char ch_val = 0;
static char my_str[13] = "             "; //"Hello, world!";



static int idx_set(const char* val, const struct kernel_param *kp)
{
    int new_val;
    int ret = kstrtoint(val, 10, &new_val);
    if(ret < 0)
    {
        pr_err("Ошибка преобразования в число параметра idx(%d)\n",ret);
        return ret;
    }

    if((new_val < 0) || (new_val >= sizeof(my_str)))
    {
        pr_err("idx вне диапазона (0-12) (%d)\n",new_val);
        return -EINVAL;
    }

    idx = new_val;

    return 0;
}

static int idx_get(char* buffer, const struct kernel_param *kp)
{
    return param_get_byte(buffer, kp);
}

static const struct kernel_param_ops idx_ops = {
    .get = idx_get,
    .set = idx_set,
};

module_param_cb(idx, &idx_ops, &idx, 0644);
MODULE_PARM_DESC(idx, "Номер символа в my_str для записи символа (0-12)");

static int ch_val_set(const char* val, const struct kernel_param *kp)
{
    int new_val;
    int ret = kstrtoint(val, 10, &new_val);
    if(ret < 0)
    {
        pr_err("Ошибка преобразования в число параметра ch_val(%d)\n",ret);
        return ret;
    }

    my_str[idx] = new_val;

    ch_val = new_val;

    return 0;
}

static int ch_val_get(char* buffer, const struct kernel_param *kp)
{
    return param_get_byte(buffer, kp);
}

static const struct kernel_param_ops ch_val_ops = {
    .get = ch_val_get,
    .set = ch_val_set,
};

module_param_cb(ch_val, &ch_val_ops, &ch_val, 0644);
MODULE_PARM_DESC(ch_val, "ASCI код символа для записи в my_str");

static int my_str_get(char *buffer, const struct kernel_param *kp)
{
    return scnprintf(buffer, PAGE_SIZE, "%s\n", my_str);
}

static const struct kernel_param_ops my_str_ops = {
    .get = my_str_get,
    .set = NULL,
};

module_param_cb(my_str, &my_str_ops, &my_str, 0444);
MODULE_PARM_DESC(my_str, "Строка с записанными символами размером 14 символов (read-only)");

static int __init hello_init(void) {
  pr_info(" init\n");
  return 0;
}

static void __exit hello_exit(void) { pr_info(" exit\n"); }

module_init(hello_init);
module_exit(hello_exit);

MODULE_LICENSE("GPL");
MODULE_AUTHOR("RYBIN ALEXANDER");
MODULE_DESCRIPTION("Simple hello world");
MODULE_VERSION("0.1");
