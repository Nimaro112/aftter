package warz.attachments
{
   public class attachmentsDebug extends Object
   {
       
      public function attachmentsDebug()
      {
         super();
      }
      
      public static function initDebug(param1:attachments) : *
      {
         param1.addAttachment(0,100,100,1);
         param1.addSlot(0,0,"Test",9,0);
         param1.addSlot(0,1,"Test 2",2,0);
         param1.setSlotActive(0,0);
         param1.addAttachment(1,200,350,2);
         param1.addSlot(1,0,"Test",65465,0);
         param1.addSlot(1,1,"Test 2",6534637,0);
         param1.addSlot(1,2,"Give me Life",54353,0);
         param1.setSlotActive(1,1);
      }
   }
}
