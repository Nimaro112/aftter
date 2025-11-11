package warz.frontend {
	import flash.display.MovieClip;
	import flash.events.MouseEvent;
	import warz.events.FrontEndEvents;
	import warz.events.SoundEvents;
	import flash.events.TextEvent;
	import warz.utils.ImageLoader;
	import flash.display.Bitmap;
	import warz.utils.Layout;
	
	public class msgBoxCase extends MovieClip {
		
		public	var	MsgboxCase:MovieClip;
		
		public	var	callback:*;
		
		public	function msgBoxCase ()
		{
			visible = false;
		}		

		public function onClickLink(e:TextEvent)
		{
			FrontEndEvents.eventOpenURL(e.text);
		}
		
		public function loadSlotIcon(path:String, imageHolder:MovieClip)
		{
			var	dat:Object = {imageHolder:imageHolder};
			var	imageLoader:ImageLoader = new ImageLoader (path, loadSlotCallback, dat)
		}
		
		public	function loadSlotCallback (bitmap:Bitmap, dat:Object)
		{
			bitmap = Layout.stretch (bitmap, 189, 117, bitmap.width, bitmap.height, "uniform");
			bitmap.x = -37;
			bitmap.y = 0;
			
			dat.imageHolder.addChild (bitmap);
		}
		
		public function showInfoMsgCase(icon:String, msg:String, OKButton:Boolean, title:String = "")
		{
			while (MsgboxCase.Pic.numChildren)
					MsgboxCase.Pic.removeChildAt(0);			
			visible = true;
			MsgboxCase.Title.text = title;			
					
			if (title == "")
			{
				MsgboxCase.Backlight.visible = false;
			}			
			if (icon != "")
					loadSlotIcon (icon, MsgboxCase.Pic);
			
		    MsgboxCase.Text.htmlText = msg;
			MsgboxCase.Text.addEventListener( TextEvent.LINK, onClickLink); 

			MsgboxCase.OKBtn.visible = OKButton;
			MsgboxCase.OKBtn.Text.Text.text = "$FR_OK";			
			
			var	me = this;
			
			if (OKButton)
			{
				MsgboxCase.OKBtn.Btn.addEventListener(MouseEvent.MOUSE_OVER, MouseOver);
				MsgboxCase.OKBtn.Btn.addEventListener(MouseEvent.MOUSE_OUT, MouseOut);
				MsgboxCase.OKBtn.Btn.addEventListener(MouseEvent.CLICK, MouseOkClick);
			}
		}		
		
		public	function MouseOver (evt:MouseEvent):void
		{
			if (evt.currentTarget.parent.currentLabel != "over")
				evt.currentTarget.parent.gotoAndPlay("over");
		}
		
		public	function MouseOut (evt:MouseEvent):void
		{
			if (evt.currentTarget.parent.currentLabel != "out")
				evt.currentTarget.parent.gotoAndPlay("out");
		}

		public	function MouseOkClick (evt:MouseEvent):void
		{
			visible = false; 
			
			SoundEvents.eventSoundPlay("menu_click");
			
			MsgboxCase.OKBtn.Btn.removeEventListener(MouseEvent.MOUSE_OVER, MouseOver);
			MsgboxCase.OKBtn.Btn.removeEventListener(MouseEvent.MOUSE_OUT, MouseOut);
			MsgboxCase.OKBtn.Btn.removeEventListener(MouseEvent.CLICK, MouseOkClick);

			FrontEndEvents.eventMsgBoxCallback ();
		}		
	}
}