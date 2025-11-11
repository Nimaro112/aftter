package warz.attachments
{
   import flash.display.MovieClip;
   import flash.events.MouseEvent;
   import flash.events.Event;
   import warz.events.AttachmentEvents;
   
   public class attachmentIcon extends MovieClip
   {
       
      public var Slot9:MovieClip;
      
      public var Slot10:MovieClip;
      
      public var Slot1:MovieClip;
      
      public var Slot2:MovieClip;
      
      public var Slot3:MovieClip;
      
      public var Slot4:MovieClip;
      
      public var Slot5:MovieClip;
      
      public var Slot6:MovieClip;
      
      public var Slot7:MovieClip;
      
      public var Slot8:MovieClip;
      
      public var Circle:MovieClip;
      
      public var slotID:int;
      
      public var SkinID:int;
      
      public function attachmentIcon()
      {
         super();
         this.init();
      }
      
      public function init(param1:int = 0) : *
      {
         var _loc3_:String = null;
         this.slotID = param1;
         var _loc2_:* = 1;
         while(_loc2_ <= 10)
         {
            _loc3_ = "Slot" + String(_loc2_);
            this[_loc3_].visible = false;
            _loc2_++;
         }
         this.Circle.Text.gotoAndStop(param1 + 1);
         this.Circle.gotoAndPlay(0);
      }
      
      public function addSlot(param1:int, param2:String, param3:int, param4:int) : void
      {
         if(param1 >= 10)
         {
            return;
         }
         var _loc5_:String = "Slot" + String(param1 + 1);
         this[_loc5_].Btn.addEventListener(MouseEvent.MOUSE_OVER,this.rollOverFn);
         this[_loc5_].Btn.addEventListener(MouseEvent.MOUSE_OUT,this.rollOutFn);
         this[_loc5_].Btn.addEventListener(MouseEvent.CLICK,this.pressFn);
         this[_loc5_].attachmentID = param3;
         this[_loc5_].slotID = param1;
         this[_loc5_].SkinID = param4;
         this[_loc5_].Text.Text.text = param2;
         this[_loc5_].visible = true;
      }
      
      public function setSlotActive(param1:int) : *
      {
         var _loc3_:String = null;
         if(param1 >= 10)
         {
            return;
         }
         var _loc2_:* = 1;
         while(_loc2_ <= 10)
         {
            _loc3_ = "Slot" + String(_loc2_);
            if(this[_loc3_].State == "active")
            {
               if(this[_loc3_].currentLabel != "off")
               {
                  this[_loc3_].gotoAndPlay("off");
               }
               this[_loc3_].State = "off";
            }
            _loc2_++;
         }
         _loc3_ = "Slot" + String(param1 + 1);
         if(this[_loc3_].currentLabel != "hilite")
         {
            this[_loc3_].gotoAndPlay("hilite");
         }
         this[_loc3_].State = "active";
      }
      
      private function rollOverFn(param1:Event) : *
      {
         if(param1.currentTarget.parent.State != "active")
         {
            param1.currentTarget.parent.gotoAndPlay("on");
         }
      }
      
      private function rollOutFn(param1:Event) : *
      {
         if(param1.currentTarget.parent.State != "active")
         {
            param1.currentTarget.parent.gotoAndPlay("off");
         }
      }
      
      private function pressFn(param1:Event) : *
      {
         var _loc2_:* = undefined;
         var _loc3_:String = null;
         if(param1.currentTarget.parent.State != "active")
         {
            _loc2_ = 1;
            while(_loc2_ <= 10)
            {
               _loc3_ = "Slot" + String(_loc2_);
               if(this[_loc3_].State == "active")
               {
                  this[_loc3_].gotoAndPlay("off");
                  this[_loc3_].State = "off";
               }
               _loc2_++;
            }
            param1.currentTarget.parent.State = "active";
            param1.currentTarget.parent.gotoAndPlay("hilite");
            AttachmentEvents.eventSelectAttachment(this.slotID,param1.currentTarget.parent.slotID,param1.currentTarget.parent.attachmentID,param1.currentTarget.parent.SkinID);
         }
      }
      
      public function removeEvents() : void
      {
         var _loc2_:* = undefined;
         var _loc1_:* = 1;
         while(_loc1_ <= 10)
         {
            _loc2_ = "Slot" + String(_loc1_);
            this[_loc2_].Btn.removeEventListener(MouseEvent.MOUSE_OVER,this.rollOverFn);
            this[_loc2_].Btn.removeEventListener(MouseEvent.MOUSE_OUT,this.rollOutFn);
            this[_loc2_].Btn.removeEventListener(MouseEvent.CLICK,this.pressFn);
            _loc1_++;
         }
      }
   }
}
