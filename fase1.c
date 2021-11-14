#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/gpio.h>
#include <linux/interrupt.h>

#include <asm/irq.h>
#include <asm/io.h>


MODULE_LICENSE("GPL");


static unsigned int gpioLED1=20; //Hardcode del pin del led1
static unsigned int gpioLED2=16; //Hardcode del pin del led3

static bool led1On=0; //Bool para determinar si estaencendido o no. Se usa como apagado por defecto. 
static bool led2On=0; //Bool para determinar si estaencendido o no. Se usa como apagado por defecto. 

static unsigned int gpioBUT1=26; //Hardcode del pin del boton1
static unsigned int gpioBUT2=19; //Hardcode del pin del boton2
static unsigned int gpioBUT3=13; //Hardcode del pin del boton3
static unsigned int gpioBUT4=21; //Hardcode del pin del boton4

static unsigned int irqNumber1; 
static unsigned int irqNumber2; 
static unsigned int irqNumber3; 
static unsigned int irqNumber4; 

static irq_handler_t  ebbgpio_irq_handler1(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t  ebbgpio_irq_handler2(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t  ebbgpio_irq_handler3(unsigned int irq, void *dev_id, struct pt_regs *regs);
static irq_handler_t  ebbgpio_irq_handler4(unsigned int irq, void *dev_id, struct pt_regs *regs);

static int numberPressed1=0;
static int numberPressed2=0;
static int numberPressed3=0;
static int numberPressed4=0;


static int __init my_init(void) {
	//Inicializacion + check de los pines de los leds. 	
	
	int result = 0;

	printk(KERN_INFO "LKM_FBG_FASE1: Initializing Fase 1 LKM \n");
	if(!gpio_is_valid(gpioLED1) || !gpio_is_valid(gpioLED2)){
		printk(KERN_INFO "LKM_FBG_FASE1: One of the LEDs Hardcoded it is not correct. Please check.  \n");
	}


   	gpio_request(gpioLED1, "sysfs");          
   	gpio_direction_output(gpioLED1, led1On);   
   	
   	gpio_request(gpioLED2, "sysfs");          
   	gpio_direction_output(gpioLED2, led2On);   
	
	gpio_export(gpioLED1, false);             
	gpio_export(gpioLED2, false);             
   	
	gpio_request(gpioBUT1, "sysfs");       
   	gpio_direction_input(gpioBUT1);        
   	gpio_set_debounce(gpioBUT1, 200);      
   	gpio_export(gpioBUT1, false);         
 	
	gpio_request(gpioBUT2, "sysfs");       
   	gpio_direction_input(gpioBUT2);        
   	gpio_set_debounce(gpioBUT2, 200);      
   	gpio_export(gpioBUT2, false);         
	
	gpio_request(gpioBUT3, "sysfs");       
   	gpio_direction_input(gpioBUT3);        
   	gpio_set_debounce(gpioBUT3, 200);      
   	gpio_export(gpioBUT3, false);         
	
       
	gpio_request(gpioBUT4, "sysfs");       
   	gpio_direction_input(gpioBUT4);        
   	gpio_set_debounce(gpioBUT4, 200);      
   	gpio_export(gpioBUT4, false);         


	// GPIO numbers and IRQ numbers are not the same! This function performs the mapping for us
	irqNumber1 = gpio_to_irq(gpioBUT1);
	irqNumber2 = gpio_to_irq(gpioBUT2);
	irqNumber3 = gpio_to_irq(gpioBUT3);
	irqNumber4 = gpio_to_irq(gpioBUT4);
 
   	// This next call requests an interrupt line
   	result = request_irq(irqNumber1,             // The interrupt number requested
                            (irq_handler_t) ebbgpio_irq_handler1, // The pointer to the handler function below
                            IRQF_TRIGGER_RISING,   // Interrupt on rising edge (button press, not release)
                            "ebb_gpio_handler",    // Used in /proc/interrupts to identify the owner
                            NULL);                 // The *dev_id for shared interrupt lines, NULL is okay
 


	result = request_irq(irqNumber2,             // The interrupt number requested
                            (irq_handler_t) ebbgpio_irq_handler2, // The pointer to the handler function below
                            IRQF_TRIGGER_RISING,   // Interrupt on rising edge (button press, not release)
                            "ebb_gpio_handler",    // Used in /proc/interrupts to identify the owner
                            NULL);                 // The *dev_id for shared interrupt lines, NULL is okay
 

	result = request_irq(irqNumber3,             // The interrupt number requested
                            (irq_handler_t) ebbgpio_irq_handler3, // The pointer to the handler function below
                            IRQF_TRIGGER_RISING,   // Interrupt on rising edge (button press, not release)
                            "ebb_gpio_handler",    // Used in /proc/interrupts to identify the owner
                            NULL);                 // The *dev_id for shared interrupt lines, NULL is okay


	result = request_irq(irqNumber4,             // The interrupt number requested
                            (irq_handler_t) ebbgpio_irq_handler4, // The pointer to the handler function below
                            IRQF_TRIGGER_RISING,   // Interrupt on rising edge (button press, not release)
                            "ebb_gpio_handler",    // Used in /proc/interrupts to identify the owner
                            NULL);                 // The *dev_id for shared interrupt lines, NULL is okay
   return result;	
	
	
	
}

static void __exit my_exit(void) {

   	
	gpio_unexport(gpioLED1);                  // Unexport the LED GPIO
	gpio_unexport(gpioLED2);                  // Unexport the LED GPIO
	
	free_irq(irqNumber1, NULL);               // Free the IRQ number, no *dev_id required in this case
	free_irq(irqNumber2, NULL);               // Free the IRQ number, no *dev_id required in this case
	free_irq(irqNumber3, NULL);               // Free the IRQ number, no *dev_id required in this case
	free_irq(irqNumber4, NULL);               // Free the IRQ number, no *dev_id required in this case
   	
	gpio_unexport(gpioBUT1);               // Unexport the Button GPIO
	gpio_unexport(gpioBUT2);               // Unexport the Button GPIO
	gpio_unexport(gpioBUT3);               // Unexport the Button GPIO
	gpio_unexport(gpioBUT4);               // Unexport the Button GPIO
	
	gpio_free(gpioLED1);                      // Free the LED GPIO
	gpio_free(gpioLED2);                      // Free the LED GPIO
   	
	gpio_free(gpioBUT1);                   // Free the Button GPIO
   	gpio_free(gpioBUT2);                   // Free the Button GPIO
   	gpio_free(gpioBUT3);                   // Free the Button GPIO
   	gpio_free(gpioBUT4);                   // Free the Button GPIO
   	
	printk(KERN_INFO "LKM_FBG_FASE1: Goodbye from the LKM!\n");


	return ;
}


static irq_handler_t ebbgpio_irq_handler1(unsigned int irq, void *dev_id, struct pt_regs *regs){
   led1On=true;                          // Invert the LED state on each button press
   gpio_set_value(gpioLED1, led1On);          // Set the physical LED accordingly
   numberPressed1++;
   printk(KERN_INFO "LKM_FBG_FASE1: Button 1 pressed. Led 1 ON.\n");
   return (irq_handler_t) IRQ_HANDLED;      // Announce that the IRQ has been handled correctly
}

static irq_handler_t ebbgpio_irq_handler2(unsigned int irq, void *dev_id, struct pt_regs *regs){
   led1On=false;                          // Invert the LED state on each button press
   gpio_set_value(gpioLED1, led1On);          // Set the physical LED accordingly
   numberPressed2++;
   printk(KERN_INFO "LKM_FBG_FASE1: Button 2 pressed. Led 1 OFF.\n");
   return (irq_handler_t) IRQ_HANDLED;      // Announce that the IRQ has been handled correctly
}

static irq_handler_t ebbgpio_irq_handler3(unsigned int irq, void *dev_id, struct pt_regs *regs){
   led2On=true;                          // Invert the LED state on each button press
   gpio_set_value(gpioLED2, led2On);          // Set the physical LED accordingly
   numberPressed3++;
   printk(KERN_INFO "LKM_FBG_FASE1: Button 3 pressed. Led 2 ON.\n");
   return (irq_handler_t) IRQ_HANDLED;      // Announce that the IRQ has been handled correctly
}

static irq_handler_t ebbgpio_irq_handler4(unsigned int irq, void *dev_id, struct pt_regs *regs){
   led2On=false;                          // Invert the LED state on each button press
   gpio_set_value(gpioLED2, led2On);          // Set the physical LED accordingly
   numberPressed4++;
   printk(KERN_INFO "LKM_FBG_FASE1: Button 4 pressed. Led 2 OFF.\n");
   return (irq_handler_t) IRQ_HANDLED;      // Announce that the IRQ has been handled correctly
}


module_init(my_init);
module_exit(my_exit);
