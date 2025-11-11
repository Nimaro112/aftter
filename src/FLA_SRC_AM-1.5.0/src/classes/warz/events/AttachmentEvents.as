package warz.events
{
   public class AttachmentEvents extends Events
   {
       
      public function AttachmentEvents()
      {
         super();
      }
      
      public static function eventSelectAttachment(param1:int, param2:int, param3:int, param4:int) : *
      {
         send("eventSelectAttachment",param1,param2,param3,param4);
      }
   }
}
