/*
Allocate a buffer in memory that is large enough to hold the data to be transferred.

Request a DMA channel from the operating system's DMA API. This will provide access to the DMA controller and its associated registers.

Initialize the DMA controller with the necessary configuration, such as the source and destination addresses, transfer size, and transfer mode.

Notify the device that a DMA transfer is about to start.

Start the DMA transfer by writing the appropriate commands to the DMA controller's registers.

Wait for the transfer to complete by polling the DMA controller's status registers or using interrupts.

Once the transfer is complete, notify the device and release the DMA channel.

Process the transferred data as required by the device driver.

*/
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/dmaengine.h>
#include <linux/wait.h>
#include <linux/interrupt.h>
#include <linux/sched.h>

#define DEVICE_NAME "dma_test"
#define DMA_BUFFER_SIZE (4096)
#define IRQ_NUM (42)

static int major_num;
static struct dma_chan *dma_channel;
static dma_cookie_t cookie;
static dma_addr_t dma_buffer_handle;
static void *dma_buffer;
static bool finished = false;
static DECLARE_WAIT_QUEUE_HEAD(wait_queue);

static irqreturn_t dma_module_irq(int irq, void *dev_id)
{
    struct device *dev = dev_id;

    if (dma_async_is_tx_complete(dma_channel, cookie, NULL, NULL) == DMA_COMPLETE) {
        finished = true;
        wake_up(&wait_queue);
    }

    return IRQ_HANDLED;
}

static void dma_module_callback(void *completion)
{
    if (dma_async_is_tx_complete(dma_channel, cookie, NULL, NULL) == DMA_COMPLETE) {
        finished = true;
        wake_up(&wait_queue);
    }
}

static int dma_module_open(struct inode *inodep, struct file *filep)
{
    struct dma_async_tx_descriptor *txdesc;
    struct device *dev = filep->private_data;
    int ret;

    dma_buffer = dma_zalloc_coherent(dev, DMA_BUFFER_SIZE, &dma_buffer_handle, GFP_KERNEL);
    if (!dma_buffer) {
        printk(KERN_ERR "Failed to allocate DMA buffer\n");
        return -ENOMEM;
    }

    dma_channel = dma_request_chan(dev, "dma_test");
    if (!dma_channel) {
        printk(KERN_ERR "Failed to request DMA channel\n");
        ret = -ENODEV;
        goto fail_dma_alloc;
    }

    ret = request_irq(IRQ_NUM, dma_module_irq, IRQF_TRIGGER_NONE, "dma_test", dev);
    if (ret) {
        printk(KERN_ERR "Failed to register IRQ handler\n");
        goto fail_dma_request;
    }

    txdesc = dmaengine_prep_slave_single(dma_channel, dma_buffer_handle, DMA_BUFFER_SIZE, DMA_MEM_TO_DEV, DMA_PREP_INTERRUPT);
    if (!txdesc) {
        printk(KERN_ERR "Failed to prepare DMA transfer\n");
        ret = -EIO;
        goto fail_irq_request;
    }

    txdesc->callback = dma_module_callback;
    cookie = dmaengine_submit(txdesc);
    dma_async_issue_pending(dma_channel);

    wait_event_interruptible(wait_queue, finished);

    free_irq(IRQ_NUM, dev);
    dma_release_channel(dma_channel);
    dma_free_coherent(dev, DMA_BUFFER_SIZE, dma_buffer, dma_buffer_handle);

    return 0;

fail_irq_request:
    free_irq(IRQ_NUM, dev);
fail_dma_request:
    dma_release_channel(dma_channel);
fail_dma_alloc:
    dma_free_coherent(dev, DMA_BUFFER_SIZE, dma_buffer, dma_buffer_handle);
    return ret;
}

static int dma_module_release(struct inode *inodep, struct file *filep)
{
    return 0;
}

static const struct file_operations fops = {
    .owner = THIS_MODULE,
    .open = dma_module_open,
    .release = dma_module_release,
};

static int __init dma_module_init(void)
{
    major_num = register_chrdev(0, DEVICE_NAME, &fops);
    if (major_num < 0) {
        printk(KERN_ERR "Failed to register character device\n");
        return major_num;
    }

    printk(KERN_INFO "DMA test module loaded\n");
    return 0;
}

static void __exit dma_module_exit(void)
{
unregister_chrdev(major_num, DEVICE_NAME);
printk(KERN_INFO "DMA test module unloaded\n");
}

module_init(dma_module_init);
module_exit(dma_module_exit);
MODULE_LICENSE("GPL");
MODULE_AUTHOR("Ananbose");
MODULE_DESCRIPTION("A DMA test module for Linux");


