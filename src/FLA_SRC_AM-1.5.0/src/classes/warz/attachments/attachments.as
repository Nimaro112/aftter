package warz.attachments
{
   import flash.display.MovieClip;
   
   public class attachments extends MovieClip
   {
      
      public static var api:attachments = null;
      
      {
         api = null;
      }
      
      public var isDebug:Boolean = false;
      
      public var Main:MovieClip = null;
      
      public function attachments(param1:MovieClip)
      {
         super();
         api = this;
         this.Main = param1;
         this.Main.init();
      }
      
      public function clearAttachments() : void
      {
         this.Main.clearAttachments();
      }
      
      public function addAttachment(param1:int, param2:int, param3:int, param4:int) : void
      {
         this.Main.addAttachment(param1,param2,param3,param4);
      }
      
      public function addSlot(param1:int, param2:int, param3:String, param4:int, param5:int) : void
      {
         this.Main.addSlot(param1,param2,param3,param4,param5);
      }
      
      public function setSlotActive(param1:int, param2:int) : *
      {
         this.Main.setSlotActive(param1,param2);
      }
   }
}
