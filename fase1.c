#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

#include <asm/irq.h>
#include <asm/io.h>


MODULE_LICENSE("FBG");


static unsigned int gpioLED1=20; //Hardcode del pin del led1
//static unsigned int gpioLED2=16; //Hardcode del pin del led3

static bool led1On=0; //Bool para determinar si estaencendido o no. Se usa como apagado por defecto. 
//static bool led2On=0; //Bool para determinar si estaencendido o no. Se usa como apagado por defecto. 

static unsigned int gpioBUT1=26; //Hardcode del pin del boton1
//static unsigned int gpioBUT2=27; //Hardcode del pin del boton2
//static unsigned int gpioBUT3=5; //Hardcode del pin del boton3
//static unsigned int gpioBUT4=6; //Hardcode del pin del boton4

static unsigned int irqNumber; 

static irq_handler_t  ebbgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs);




static int __init my_init(void) {
	//Inicializacion + check de los pines de los leds. 	
	
	int result = 0;

	printk(KERN_INFO "FASE 1 : Initializing Fase 1 LKM \n");
	//if(!gpio_is_valid(gpioLED1) || !gpio_is_valid(gpioLED2)){
	if(!gpio_is_valid(gpioLED1) ){
		printk(KERN_INFO "FASE 1 :One of the LEDs Hardcoded it is not correct. Please check.  \n");
	}


	led1On= true;
   	gpio_request(gpioLED1, "sysfs");          
   	gpio_direction_output(gpioLED1, led1On);   
   	gpio_export(gpioLED1, false);             
   	gpio_request(gpioBUT1, "sysfs");       
   	gpio_direction_input(gpioBUT1);        
   	gpio_set_debounce(gpioBUT1, 200);      
   	gpio_export(gpioBUT1, false);         
   	printk(KERN_INFO "GPIO_TEST: The button state is currently: %d\n", gpio_get_value(gpioBUT1));
 	
	
	 // GPIO numbers and IRQ numbers are not the same! This function performs the mapping for us
	irqNumber = gpio_to_irq(gpioBUT1);
   	printk(KERN_INFO "GPIO_TEST: The button is mapped to IRQ: %d\n", irqNumber);
 
   // This next call requests an interrupt line
   result = request_irq(irqNumber,             // The interrupt number requested
                        (irq_handler_t) ebbgpio_irq_handler, // The pointer to the handler function below
                        IRQF_TRIGGER_RISING,   // Interrupt on rising edge (button press, not release)
                        "ebb_gpio_handler",    // Used in /proc/interrupts to identify the owner
                        NULL);                 // The *dev_id for shared interrupt lines, NULL is okay
 
   printk(KERN_INFO "GPIO_TEST: The interrupt request result is: %d\n", result);
   return result;	
	
	
	
}

static void __exit my_exit(void) {

	printk(KERN_INFO "GPIO_TEST: The button state is currently: %d\n", gpio_get_value(gpioBUT1));
   	gpio_set_value(gpioLED1, 0);              // Turn the LED off, makes it clear the device was unloaded
   	
	//gpio_unexport(gpioLED1);                  // Unexport the LED GPIO
	free_irq(irqNumber, NULL);               // Free the IRQ number, no *dev_id required in this case
   	
	//gpio_unexport(gpioBUT1);               // Unexport the Button GPIO
   	
	//gpio_free(gpioLED1);                      // Free the LED GPIO
   	//gpio_free(gpioBUT1);                   // Free the Button GPIO
   	
	printk(KERN_INFO "GPIO_TEST: Goodbye from the LKM!\n");

	return ;
}


static irq_handler_t ebbgpio_irq_handler(unsigned int irq, void *dev_id, struct pt_regs *regs){
   led1On = !led1On;                          // Invert the LED state on each button press
   gpio_set_value(gpioLED1, led1On);          // Set the physical LED accordingly
   printk(KERN_INFO "GPIO_TEST: Interrupt! (button state is %d)\n", gpio_get_value(gpioBUT1));
   return (irq_handler_t) IRQ_HANDLED;      // Announce that the IRQ has been handled correctly
}




module_init(my_init);
module_exit(my_exit);
