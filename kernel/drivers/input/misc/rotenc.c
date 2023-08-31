#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <linux/input.h>
#include <linux/device.h>
#include <linux/platform_device.h>
#include <linux/gpio/consumer.h>
#include <linux/slab.h>
#include <linux/of.h>
#include <linux/pm.h>
#include <linux/property.h>
#include <linux/gpio.h>
#include <linux/moduleparam.h>
#include <linux/rtc.h>
#include <linux/vmalloc.h>
#include <linux/types.h>
#include <linux/sched.h>
#include <linux/delay.h>
#include <linux/workqueue.h>
#include <linux/of_platform.h>
#include <linux/of_gpio.h>
#include <linux/of_irq.h>

	
#define DRV_NAME "rotenc"

struct myGpio_desc {
	unsigned int ndescs;
	struct gpio_desc *desc[3];
};

#define SOFTWARE_DEBOUNCE_MS 100

struct rotary_encoder {
	struct input_dev *input;
	struct mutex access_mutex;
	unsigned int pos;
	struct myGpio_desc *gpios;
	unsigned int *irq;
	unsigned int type;
	unsigned int last_status;
	struct timer_list release_timer;
	unsigned int currentIrq;
	spinlock_t lock;
	unsigned int software_debounce;
	bool key_pressed;
	struct work_struct work;
};

enum {
	ROTENC_DIR_LEFT,
	ROTENC_DIR_RIGHT,
};

static unsigned int rotary_encoder_get_state(struct rotary_encoder *encoder)
{
	int i;
	unsigned int ret = 0;

	for (i = 0; i < encoder->gpios->ndescs; ++i) {
		int val = gpiod_get_value_cansleep(encoder->gpios->desc[i]);
		pr_info("%d value %d\n" , i , val);
		/* convert from gray encoding to normal */

		ret = ret << 1 | val;
	}
/*
	if(ret & 0x04) {
		gpio_set_value(8 , 1);
	}
	else {
		gpio_set_value(8 , 0);
	}
*/
	return ret & 7;
}

static void rotary_encoder_report_event(struct rotary_encoder *encoder , int dir)
{
	// unsigned int type = EV_KEY;
	if(dir) {
		
		// input_report_rel(encoder->input , ABS_X , 1);
		input_report_key(encoder->input , KEY_LEFT , 1);
		input_sync(encoder->input);
		input_report_key(encoder->input , KEY_LEFT , 0);
		input_sync(encoder->input);
	}
	else {
		// input_report_rel(encoder->input , ABS_X , -1);
		input_report_key(encoder->input , KEY_RIGHT , 1);
		input_sync(encoder->input);
		input_report_key(encoder->input , KEY_RIGHT , 0);
		input_sync(encoder->input);
	}
	
}


static irqreturn_t rotary_encoder_irq(int irq, void *dev_id) 
{
	struct rotary_encoder *encoder = dev_id;
	mutex_lock(&encoder->access_mutex);
	
	encoder->currentIrq = irq;
	//mod_delayed_work(system_wq , &encoder->work , msecs_to_jiffies(encoder->software_debounce));
	schedule_work(&encoder->work);
/*
	status = rotary_encoder_get_state(encoder);
	printk("irq %d , irq1 %d, irq2 %d" , irq , encoder->irq[0] , encoder->irq[1]);
	if((irq == encoder->irq[0])) {			//gpio10上升沿中断 && (status & 0x02)
		if(status & 0x01) {
			rotary_encoder_report_event(encoder,ROTENC_DIR_LEFT);
		}
		else {
			rotary_encoder_report_event(encoder,ROTENC_DIR_RIGHT);
		}
	}
	else if((irq == encoder->irq[1])) {		//gpio11上升沿中断  && (status & 0x01)
		if(status & 0x02) {
			rotary_encoder_report_event(encoder,ROTENC_DIR_RIGHT);
		}
		else {
			rotary_encoder_report_event(encoder,ROTENC_DIR_LEFT);
		}
	}*/
	mutex_unlock(&encoder->access_mutex);
	return IRQ_HANDLED;
}

int irq1_low_flag; //中断1低电平标志

static void gpio_keys_gpio_work_func(struct work_struct *work)
{
	struct rotary_encoder *encoder = container_of(work , struct rotary_encoder , work);
	unsigned int status;
	// mutex_lock(&encoder->access_mutex);
	
	status = rotary_encoder_get_state(encoder);
	printk("encoder currentStatus %d , last_status %d\n" , status , encoder->last_status);
	if(status == encoder->last_status && encoder->currentIrq != encoder->irq[0]) {
		// mutex_unlock(&encoder->access_mutex);
		return ;
	}
    if(encoder->currentIrq == encoder->irq[0]) {
        gpio_set_value(8 , (status & 0x04)?1:0);
        if(status & 0x04) {
            unsigned long flags;
            spin_lock_irqsave(&encoder->lock, flags);

            irq_set_irq_type(encoder->irq[0] ,  IRQF_TRIGGER_FALLING);

            if (false == encoder->key_pressed) {
                input_report_key(encoder->input , KEY_ENTER , 1);
                input_sync(encoder->input);

                encoder->key_pressed = true;

                mod_timer(&encoder->release_timer,
                    jiffies + msecs_to_jiffies(encoder->software_debounce));
            }

            spin_unlock_irqrestore(&encoder->lock, flags);
        }
        else {
            irq_set_irq_type(encoder->irq[0] , IRQF_TRIGGER_RISING);
            input_report_key(encoder->input , KEY_ENTER , 0);
            input_sync(encoder->input);
        }
    }
	else if((encoder->currentIrq == encoder->irq[1])) {			//gpio10上升沿中断 && (status & 0x02)
		if(status & 0x01) {
			irq1_low_flag = 0;//高电平
		}
		else {
			irq1_low_flag = 1; //低电平
		}
	}
	else if((encoder->currentIrq == encoder->irq[2])) {		//gpio11上升沿中断  && (status & 0x01)
		if (status & 0x02) {
			// 一个中断周期内，irq1为低电平， irq2为高电平，为逆时针
			if (irq1_low_flag) {
				rotary_encoder_report_event(encoder,ROTENC_DIR_RIGHT);
			}
		} else {
			// 一个中断周期内，irq1为高电平，irq2为低电平，为顺时针
			if (!irq1_low_flag) {
				rotary_encoder_report_event(encoder,ROTENC_DIR_LEFT);
			}
		}
	}
	encoder->last_status = status;
	// mutex_unlock(&encoder->access_mutex);
}

static void gpio_keys_quiesce_key(void *data) 
{
	struct rotary_encoder *encoder = data;
	cancel_work_sync(&encoder->work);
}

static void gpio_keys_irq_timer(unsigned long _data)
{
	struct rotary_encoder *encoder = (struct rotary_encoder *)_data;
	unsigned long flags;

	spin_lock_irqsave(&encoder->lock, flags);
	if (encoder->key_pressed) {
		encoder->key_pressed = false;
	}
	spin_unlock_irqrestore(&encoder->lock, flags);
}

static int rotenc_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct rotary_encoder *encoder;
	struct input_dev *input;
//	irq_handler_t handler;
//	u32 steps_per_period;
	unsigned int i;
	int err , keystatus;
	unsigned long irqflags;
	encoder = devm_kzalloc(dev, sizeof(struct rotary_encoder), GFP_KERNEL);
	mutex_init(&encoder->access_mutex);
	encoder->gpios = kmalloc(sizeof(struct myGpio_desc) , GFP_KERNEL);
	encoder->gpios->ndescs = 3;
	encoder->gpios->desc[0] = gpio_to_desc(9);
	encoder->gpios->desc[1] = gpio_to_desc(10);
	encoder->gpios->desc[2] = gpio_to_desc(11);
	
	gpio_direction_input(9);
	gpio_direction_input(10);
	gpio_direction_input(11);

	gpio_direction_output(8,1);
	
	input = devm_input_allocate_device(dev);
	if (!input)
	{
		printk("%s %d\n" , __FUNCTION__ , __LINE__);
		return -ENOMEM;
	}
	encoder->input = input;
	encoder->type = EV_KEY;
	input->name = DRV_NAME;
	input->id.bustype = BUS_HOST;
	input->dev.parent = dev;
	set_bit(EV_KEY , input->evbit);
	set_bit(KEY_LEFT , input->keybit);
	set_bit(KEY_RIGHT , input->keybit);
	set_bit(KEY_ENTER , input->keybit);
	set_bit(KEY_LEFT , input->key);
	set_bit(KEY_RIGHT , input->key);
	set_bit(KEY_ENTER , input->key);
	encoder->irq =
		devm_kzalloc(dev,
			     sizeof(*encoder->irq) * encoder->gpios->ndescs,
			     GFP_KERNEL);
	if (!encoder->irq)
		return -ENOMEM;
	
	//INIT_DELAYED_WORK(&encoder->work , gpio_keys_gpio_work_func);
	INIT_WORK(&encoder->work , gpio_keys_gpio_work_func);

	err = devm_add_action(dev , gpio_keys_quiesce_key , encoder);
	if(err) {
		dev_err(dev , "fail to register quiesce action error: %d\n" , err);
		return err;
	}

	for (i = 0; i < encoder->gpios->ndescs; ++i) {
		encoder->irq[i] = gpiod_to_irq(encoder->gpios->desc[i]);
		keystatus = gpiod_get_value_cansleep(encoder->gpios->desc[i]);
		if(i == 0) {
		 	if(keystatus) {
		 		irqflags = IRQF_TRIGGER_FALLING  | IRQF_ONESHOT;
		 	}
		 	else {				
		 		irqflags = IRQF_TRIGGER_RISING  | IRQF_ONESHOT;
		 	}
		 }
		 else {
			irqflags = IRQF_TRIGGER_RISING | IRQF_ONESHOT;
		 }
		err = devm_request_any_context_irq(dev , encoder->irq[i] , rotary_encoder_irq , irqflags , DRV_NAME , encoder);
		// err = devm_request_threaded_irq(dev, encoder->irq[i],
		// 		NULL, rotary_encoder_irq,
		// 		irqflags,
		// 		DRV_NAME, encoder);
		if (err) {
			dev_err(dev, "unable to request IRQ %d (gpio#%d)\n",
				encoder->irq[i], i);
			return err;
		}
	}
	encoder->software_debounce = SOFTWARE_DEBOUNCE_MS;
	encoder->key_pressed = false;
	spin_lock_init(&encoder->lock);
	setup_timer(&encoder->release_timer,
		    gpio_keys_irq_timer, (unsigned long)encoder);

	err = input_register_device(input);
	if (err) {
		dev_err(dev, "failed to register input device\n");
		return err;
	}

	// device_init_wakeup(dev,
			   // device_property_read_bool(dev, "wakeup-source"));

	platform_set_drvdata(pdev, encoder);

	return 0;
}

static int __maybe_unused rotary_encoder_suspend(struct device *dev)
{
	return 0;
}

static int __maybe_unused rotary_encoder_resume(struct device *dev)
{
	return 0;
}

static SIMPLE_DEV_PM_OPS(rotary_encoder_pm_ops,
			 rotary_encoder_suspend, rotary_encoder_resume);

static const struct of_device_id rotary_encoder_of_match[] = {
	{ .compatible = "sstar,rotary-encoder", },
	{ },
};
MODULE_DEVICE_TABLE(of, rotary_encoder_of_match);

static struct platform_driver rotenc_driver = {
	.probe		= rotenc_probe,
	.driver		= {
		.name	= DRV_NAME,
		.pm	= &rotary_encoder_pm_ops,
		.of_match_table = of_match_ptr(rotary_encoder_of_match),
	}
};
module_platform_driver(rotenc_driver);

MODULE_ALIAS("platform:" DRV_NAME);
MODULE_DESCRIPTION("GPIO rotary encoder driver");
MODULE_AUTHOR("Daniel Mack <daniel@caiaq.de>, Johan Hovold");
MODULE_LICENSE("GPL v2");
