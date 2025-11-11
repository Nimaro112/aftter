package warz.attachments
{
   import flash.display.MovieClip;
   import warz.events.SoundEvents;
   
   public class main extends MovieClip
   {
       
      public var test:attachmentIcon;
      
      public var api:attachments = null;
      
      public var Attachments:Array;
      
      public function main()
      {
         super();
         this.test.visible = false;
      }
      
      public function init() : void
      {
         this.api = attachments.api;
         this.Attachments = new Array();
      }
      
      public function clearAttachments() : void
      {
         var _loc1_:* = 0;
         while(_loc1_ < this.Attachments.length)
         {
            this.removeAttachment(_loc1_);
            _loc1_++;
         }
      }
      
      public function removeAttachment(param1:int) : void
      {
         var _loc2_:attachmentIcon = this.Attachments[param1];
         if(_loc2_)
         {
            removeChild(_loc2_);
            _loc2_.removeEvents();
            this.Attachments[param1] = null;
         }
      }
      
      public function addAttachment(param1:int, param2:int, param3:int, param4:int) : void
      {
         var _loc5_:attachmentIcon = new attachmentIcon();
         _loc5_.init(param4);
         _loc5_.x = param2;
         _loc5_.y = param3;
         this.Attachments[param1] = _loc5_;
         addChild(_loc5_);
      }
      
      public function addSlot(param1:int, param2:int, param3:String, param4:int, param5:int) : void
      {
         var _loc6_:attachmentIcon = this.Attachments[param1];
         if(_loc6_)
         {
            _loc6_.addSlot(param2,param3,param4,param5);
         }
      }
      
      public function setSlotActive(param1:int, param2:int) : *
      {
         var _loc3_:attachmentIcon = this.Attachments[param1];
         if(_loc3_)
         {
            _loc3_.setSlotActive(param2);
            SoundEvents.eventSoundPlay("weapon_attach");
         }
      }
   }
}
