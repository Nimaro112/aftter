package warz.frontend {
	import flash.display.MovieClip;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import warz.events.FrontEndEvents;
	import warz.events.SoundEvents;
	import flash.geom.Point;
	
	public class OptionsExtra extends MovieClip {
		public var isActiveScreen:Boolean = false;

		public var	OptionsExtraClip:MovieClip;
		public var	api:warz.frontend.Frontend=null;

		//AlexRedd:: Extra options
		public var Opt_ChatSound :Object; //ChatSound		
		public var Opt_Crosshair :Object; //Opt_Crosshair
		public var Opt_Highlight :Object; //Opt_Highlight
		public var Opt_IncreaseFPS :Object; // Increase FPS
		public var Opt_DisableGrass :Object; // Opt_DisableGrass
		public var Opt_JumpSound :Object; //Opt_JumpSound
		public var Opt_AlphaSound :Object;//Opt_AlphaSound
		public var Opt_KillFeedMsgOnOff :Object;//Opt_KillFeedMsgOnOff
		public var Opt_KillStreakSndOnOff :Object;//Opt_KillStreakSndOnOff		
		public var Opt_KillStreakSound :Object;//Opt_KillStreakSound
		public var Opt_CrosshaireColor :Object;//Opt_CrosshaireColor
		
		public var arNY :Array; // No, Yes
		public var arCrossHaireType :Array; // old, 1, 2, 3, 4, 5, 6, 7, 8
		public var arCrossHaireColor :Array; //"white", "red", "blue", "green"
		public var arHighlight :Array; // old, 1, 2, 3, 4, 5, 6, 7
		public var arCrossQuake :Array; //CrossFire, Quake, Duke		
		
		public	var	isMouseDown:Boolean;
		public	var	downTarget:MovieClip;

		public	function OptionsExtra ()
		{
			OptionsExtraClip.General_Options.Btn.addEventListener(MouseEvent.MOUSE_OVER, SlotRollOverFn);
			OptionsExtraClip.General_Options.Btn.addEventListener(MouseEvent.MOUSE_OUT, SlotRollOutFn);
			OptionsExtraClip.General_Options.Btn.addEventListener(MouseEvent.CLICK, SlotPressFn);
			
			OptionsExtraClip.Controls.Btn.addEventListener(MouseEvent.MOUSE_OVER, SlotRollOverFn);
			OptionsExtraClip.Controls.Btn.addEventListener(MouseEvent.MOUSE_OUT, SlotRollOutFn);
			OptionsExtraClip.Controls.Btn.addEventListener(MouseEvent.CLICK, SlotPressFn);
		
			OptionsExtraClip.BtnBack.Btn.addEventListener(MouseEvent.MOUSE_OVER, SlotRollOverFn);
			OptionsExtraClip.BtnBack.Btn.addEventListener(MouseEvent.MOUSE_OUT, SlotRollOutFn);
			OptionsExtraClip.BtnBack.Btn.addEventListener(MouseEvent.CLICK, SlotPressFn);
			
			OptionsExtraClip.Language.Btn.addEventListener(MouseEvent.MOUSE_OVER, SlotRollOverFn);
			OptionsExtraClip.Language.Btn.addEventListener(MouseEvent.MOUSE_OUT, SlotRollOutFn);
			OptionsExtraClip.Language.Btn.addEventListener(MouseEvent.CLICK, SlotPressFn);

			OptionsExtraClip.Voip.Btn.addEventListener(MouseEvent.MOUSE_OVER, SlotRollOverFn);
			OptionsExtraClip.Voip.Btn.addEventListener(MouseEvent.MOUSE_OUT, SlotRollOutFn);
			OptionsExtraClip.Voip.Btn.addEventListener(MouseEvent.CLICK, SlotPressFn);

			OptionsExtraClip.BtnReset.Btn.addEventListener(MouseEvent.MOUSE_OVER, SlotRollOverFn);
			OptionsExtraClip.BtnReset.Btn.addEventListener(MouseEvent.MOUSE_OUT, SlotRollOutFn);
			OptionsExtraClip.BtnReset.Btn.addEventListener(MouseEvent.CLICK, SlotPressFn);
			
			OptionsExtraClip.BtnApply.Btn.addEventListener(MouseEvent.MOUSE_OVER, SlotRollOverFn);
			OptionsExtraClip.BtnApply.Btn.addEventListener(MouseEvent.MOUSE_OUT, SlotRollOutFn);
			OptionsExtraClip.BtnApply.Btn.addEventListener(MouseEvent.CLICK, SlotPressFn);
			
			OptionsExtraClip.Extra.Btn.addEventListener(MouseEvent.MOUSE_OVER, SlotRollOverFn);
			OptionsExtraClip.Extra.Btn.addEventListener(MouseEvent.MOUSE_OUT, SlotRollOutFn);
			OptionsExtraClip.Extra.Btn.addEventListener(MouseEvent.CLICK, SlotPressFn);
	
			OptionsExtraClip.BtnReset.Text.Text.text = "$FR_Reset";
			OptionsExtraClip.BtnApply.Text.Text.text = "$FR_Apply";	

			//AlexRedd:: Extra options
			Opt_ChatSound = new Object(); //ChatSound			
			Opt_Crosshair = new Object(); //Opt_Crosshair
			Opt_Highlight = new Object(); //Opt_Highlight
			Opt_IncreaseFPS = new Object(); // Increase FPS
			Opt_DisableGrass = new Object(); // Opt_DisableGrass
			Opt_JumpSound = new Object(); // Opt_JumpSound
			Opt_AlphaSound = new Object(); // Opt_AlphaSound
			Opt_KillFeedMsgOnOff = new Object(); // Opt_KillFeedMsgOnOff
			Opt_KillStreakSndOnOff = new Object(); // Opt_KillStreakSndOnOff
			Opt_KillStreakSound = new Object(); // Opt_KillStreakSound
			Opt_CrosshaireColor = new Object(); // Opt_CrosshaireColor
			
			arNY = new Array("$FR_optNo", "$FR_optYes");
			arCrossHaireType = new Array("1", "2", "3", "4", "5", "6", "7", "8");
			arCrossHaireColor = new Array("0", "1", "2", "3");
			arHighlight = new Array("1", "2", "3", "4", "5", "6", "7");
			arCrossQuake = new Array("$FR_optCross", "$FR_optQuake", "$FR_optDuke");			
		}
		
		public function Activate()
		{
			api = warz.frontend.Frontend.api;
			
			this.gotoAndPlay("start");
			
			resetValues();
			
			OptionsExtraClip.Extra.State = "active";
			if (OptionsExtraClip.Extra.currentLable != "pressed")
				OptionsExtraClip.Extra.gotoAndPlay("pressed");

			api.setButtonText (OptionsExtraClip.General_Options.Text, "$FR_optGeneral");
			api.setButtonText (OptionsExtraClip.Controls.Text, "$FR_optControls");
			api.setButtonText (OptionsExtraClip.Language.Text, "$FR_optLanguage");
			api.setButtonText (OptionsExtraClip.Voip.Text, "$FR_optVoip");
			api.setButtonText (OptionsExtraClip.Extra.Text, "$FR_optExtra");
		}
		
		public function Deactivate()
		{
			this.gotoAndPlay("end");
		}
		
		private function SlotRollOverFn(evt:Event)
		{
			if(evt.currentTarget.parent.State != "active")
				evt.currentTarget.parent.gotoAndPlay("over");
		}
		
		private function SlotRollOutFn(evt:Event)
		{
			if(evt.currentTarget.parent.State != "active")
				evt.currentTarget.parent.gotoAndPlay("out");
		}
		
		private function SlotPressFn(evt:Event)
		{
			if(evt.currentTarget.parent.State != "active")
			{
				evt.currentTarget.parent.State = "active";
				evt.currentTarget.parent.gotoAndPlay("pressed");
				
				var	p:MovieClip = evt.currentTarget.parent;
				
				while (!(p as OptionsExtra))
				{
					p = p.parent as MovieClip;
				}
				
				SoundEvents.eventSoundPlay("menu_click");
				
				(p as OptionsExtra).ActionFunction(evt.currentTarget.parent.name);
			}
		}
		
		public function ActionFunction (button:String)
		{
			if (button == "BtnBack")
			{
				OptionsExtraClip.BtnBack.State = "off";
				OptionsExtraClip.BtnBack.gotoAndPlay("out");

				api.Main.showScreen("Survivors");
			}
			else if (button == "General_Options")
			{
				api.showOptionsGeneral();
				
				OptionsExtraClip.General_Options.State = "off";
				OptionsExtraClip.General_Options.gotoAndPlay("out");
			}
			else if (button == "Controls")
			{
				api.showOptionsControls();

				OptionsExtraClip.Controls.State = "off";
				OptionsExtraClip.Controls.gotoAndPlay("out");
			}
			else if (button == "Language")
			{
				api.showOptionsLanguage();

				OptionsExtraClip.Language.State = "off";
				OptionsExtraClip.Language.gotoAndPlay("out");
			}
			else if (button == "Voip")
			{
				api.showOptionsVoip();

				OptionsExtraClip.Voip.State = "off";
				OptionsExtraClip.Voip.gotoAndPlay("out");
			}
			else if (button == "Extra")
			{
				//api.showOptionsExtra();

				OptionsExtraClip.Extra.State = "off";
				OptionsExtraClip.Extra.gotoAndPlay("out");
			}
			else if (button == "BtnApply")
			{
				OptionsExtraClip.BtnApply.State = "off";
				OptionsExtraClip.BtnApply.gotoAndPlay("out");
				
				api.showInfoOkCancelMsg("$FR_ApplySettingAreYouSure", "", ApplyBtnCallBack); 
			}
			else if (button == "BtnReset")
			{
				OptionsExtraClip.BtnReset.State = "off";
				OptionsExtraClip.BtnReset.gotoAndPlay("out");
				
				api.showInfoOkCancelMsg("$FR_ResetSettingAreYouSure", "", function(isReset:Boolean){
										if(isReset)FrontEndEvents.eventOptionsReset();});
										
			}
		}
		
		
		public function ApplyBtnCallBack(isApply:Boolean):void{
			if(isApply){				

				FrontEndEvents.eventExtraOptionsApply(Opt_ChatSound.myValue+1, Opt_Crosshair.myValue+1,
				Opt_Highlight.myValue+1, Opt_IncreaseFPS.myValue+1, Opt_DisableGrass.myValue+1, Opt_JumpSound.myValue+1,
				Opt_AlphaSound.myValue+1, Opt_KillFeedMsgOnOff.myValue+1, Opt_KillStreakSndOnOff.myValue+1,
				Opt_KillStreakSound.myValue+1, Opt_CrosshaireColor.myValue+1); 
			}
		}

		private function arrowRollOverFn(evt:Event)
		{
			if(evt.currentTarget.parent.State != "active")
				evt.currentTarget.parent.gotoAndPlay("over");
		}
		
		private function arrowRollOutFn(evt:Event)
		{
			if(evt.currentTarget.parent.State != "active")
				evt.currentTarget.parent.gotoAndPlay("out");
		}
		
		private function onArrowClick(incr:Number, variable:Object, myArray:Array, Arrow:MovieClip)
		{
			var	p:MovieClip = (Arrow.parent as MovieClip);
			if(p.State == "disabled")
				return;
				
			var myVar:Number = variable.myValue;
			var prevMode:Number = myVar;
			myVar += incr;
			if(myVar<0)
				myVar = myArray.length-1;
			if(myVar>=myArray.length)
				myVar = 0;
	
			p.Value.Value1 = myArray[prevMode];
			p.Value.Value2 = myArray[myVar];
//			p.Value.Text.text = myArray[myVar];
			p.Value.gotoAndPlay ("start");
			variable.myValue = myVar;
		}
				
		private function arrowPressFn(evt:Event)
		{
			if(evt.currentTarget.parent.State != "active")
			{
//				evt.currentTarget.parent.State = "active";
//				evt.currentTarget.parent.gotoAndPlay("pressed");
				
				evt.currentTarget.parent.ActionFunction ();
			}
		}
		
		public	function resetValues ()
		{
			var	me = this;
			
			api = warz.frontend.Frontend.api;
			
			//AlexRedd:: Extra options
			Opt_ChatSound.myValue = api.Opt_ChatSound-1; //ChatSound			
			Opt_Crosshair.myValue = api.Opt_Crosshair-1; // Opt_Crosshair
			Opt_Highlight.myValue = api.Opt_Highlight-1; // Opt_Highlight
			Opt_IncreaseFPS.myValue = api.Opt_IncreaseFPS-1; // Increase FPS
			Opt_DisableGrass.myValue = api.Opt_DisableGrass-1; // Opt_DisableGrass
			Opt_JumpSound.myValue = api.Opt_JumpSound-1; // Opt_JumpSound
			Opt_AlphaSound.myValue = api.Opt_AlphaSound-1; // Opt_AlphaSound
			Opt_KillFeedMsgOnOff.myValue = api.Opt_KillFeedMsgOnOff-1; // Opt_KillFeedMsgOnOff
			Opt_KillStreakSndOnOff.myValue = api.Opt_KillStreakSndOnOff-1; // Opt_KillStreakSndOnOff			
			Opt_KillStreakSound.myValue = api.Opt_KillStreakSound-1; // Opt_KillStreakSound
			Opt_CrosshaireColor.myValue = api.Opt_CrosshaireColor-1; // Opt_CrosshaireColor
			
// ChatSound		
			 OptionsExtraClip.OptStat1.Name.text = "$FR_optChatSound";
             OptionsExtraClip.OptStat1.Value.Text.text = arNY[Opt_ChatSound.myValue];
			// OptionsExtraClip.OptStat10.Value.gotoAndPlay("start");
             OptionsExtraClip.OptStat1.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat1.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat1.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
             OptionsExtraClip.OptStat1.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat1.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat1.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
             OptionsExtraClip.OptStat1.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_ChatSound, me.arNY, this); }
             OptionsExtraClip.OptStat1.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_ChatSound, me.arNY, this); } 
			
// Crosshair new option
			OptionsExtraClip.OptStat2.Name.text = "$FR_optCrosshair";			
			OptionsExtraClip.OptStat2.Value.visible = false;
			OptionsExtraClip.OptStat2.reticle.gotoAndStop(Opt_Crosshair.myValue + 1);
			OptionsExtraClip.OptStat2.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			OptionsExtraClip.OptStat2.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			OptionsExtraClip.OptStat2.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);			

			OptionsExtraClip.OptStat2.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			OptionsExtraClip.OptStat2.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			OptionsExtraClip.OptStat2.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			OptionsExtraClip.OptStat2.ArrowLeft.ActionFunction = function() 
				{ 
					me.onArrowClick(-1, me.Opt_Crosshair, me.arCrossHaireType, this);
				    OptionsExtraClip.OptStat2.reticle.gotoAndStop(Opt_Crosshair.myValue + 1);
				}
            OptionsExtraClip.OptStat2.ArrowRight.ActionFunction = function() 
				{ 
					me.onArrowClick(+1, me.Opt_Crosshair, me.arCrossHaireType, this);
					OptionsExtraClip.OptStat2.reticle.gotoAndStop(Opt_Crosshair.myValue + 1);
				}
			
// Highlight new option
			OptionsExtraClip.OptStat3.Name.text = "$FR_optHighlight";			
			OptionsExtraClip.OptStat3.Value.visible = false;
			OptionsExtraClip.OptStat3.coloricon.gotoAndStop(Opt_Highlight.myValue + 1);
			OptionsExtraClip.OptStat3.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			OptionsExtraClip.OptStat3.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			OptionsExtraClip.OptStat3.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);			

			OptionsExtraClip.OptStat3.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			OptionsExtraClip.OptStat3.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			OptionsExtraClip.OptStat3.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			OptionsExtraClip.OptStat3.ArrowLeft.ActionFunction = function() 
				{ 
					me.onArrowClick(-1, me.Opt_Highlight, me.arHighlight, this);
				    OptionsExtraClip.OptStat3.coloricon.gotoAndStop(Opt_Highlight.myValue + 1);
				}
            OptionsExtraClip.OptStat3.ArrowRight.ActionFunction = function() 
				{ 
					me.onArrowClick(+1, me.Opt_Highlight, me.arHighlight, this);
					OptionsExtraClip.OptStat3.coloricon.gotoAndStop(Opt_Highlight.myValue + 1);
				}
// Increase FPS 
			 OptionsExtraClip.OptStat4.Name.text = "$FR_optIncreaseFPS";
             OptionsExtraClip.OptStat4.Value.Text.text = arNY[Opt_IncreaseFPS.myValue];

             OptionsExtraClip.OptStat4.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat4.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat4.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat4.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat4.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat4.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat4.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_IncreaseFPS, me.arNY, this); processIncreaseFPSSelection ();}
             OptionsExtraClip.OptStat4.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_IncreaseFPS, me.arNY, this); processIncreaseFPSSelection ();}	

// DisableGrass	 
			 OptionsExtraClip.OptStat5.Name.text = "$FR_optDisableGrass";
             OptionsExtraClip.OptStat5.Value.Text.text = arNY[Opt_DisableGrass.myValue];

             OptionsExtraClip.OptStat5.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat5.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat5.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat5.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat5.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat5.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat5.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_DisableGrass, me.arNY, this); }
             OptionsExtraClip.OptStat5.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_DisableGrass, me.arNY, this); }	

// JumpSound	 
			 OptionsExtraClip.OptStat6.Name.text = "$FR_optJumpSound";
             OptionsExtraClip.OptStat6.Value.Text.text = arNY[Opt_JumpSound.myValue];

             OptionsExtraClip.OptStat6.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat6.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat6.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat6.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat6.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat6.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat6.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_JumpSound, me.arNY, this); }
             OptionsExtraClip.OptStat6.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_JumpSound, me.arNY, this); }	

// AlphaSound	 
			 OptionsExtraClip.OptStat7.Name.text = "$FR_optAlphaSound";
             OptionsExtraClip.OptStat7.Value.Text.text = arNY[Opt_AlphaSound.myValue];

             OptionsExtraClip.OptStat7.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat7.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat7.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat7.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat7.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat7.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat7.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_AlphaSound, me.arNY, this); }
             OptionsExtraClip.OptStat7.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_AlphaSound, me.arNY, this); }	

// KillFeedMsgOnOff	 
			 OptionsExtraClip.OptStat8.Name.text = "$FR_optKillFeedMsgOnOff";
             OptionsExtraClip.OptStat8.Value.Text.text = arNY[Opt_KillFeedMsgOnOff.myValue];

             OptionsExtraClip.OptStat8.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat8.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat8.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat8.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat8.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat8.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat8.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_KillFeedMsgOnOff, me.arNY, this); }
             OptionsExtraClip.OptStat8.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_KillFeedMsgOnOff, me.arNY, this); }				 

//KillStreakSndOnOff 
			 OptionsExtraClip.OptStat9.Name.text = "$FR_optKillStreakSndOnOff";
             OptionsExtraClip.OptStat9.Value.Text.text = arNY[Opt_KillStreakSndOnOff.myValue];

             OptionsExtraClip.OptStat9.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat9.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat9.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat9.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat9.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat9.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat9.ArrowLeft.ActionFunction = function() 
				{ 
					me.onArrowClick(-1, me.Opt_KillStreakSndOnOff, me.arNY, this); 
					processKillSreakSelection ();
				}
             OptionsExtraClip.OptStat9.ArrowRight.ActionFunction = function() 
				{ 
					me.onArrowClick(+1, me.Opt_KillStreakSndOnOff, me.arNY, this); 
					processKillSreakSelection ();
				}

// KillStreakSound 
			 OptionsExtraClip.OptStat10.Name.text = "$FR_optKillStreakSound";
             OptionsExtraClip.OptStat10.Value.Text.text = arCrossQuake[Opt_KillStreakSound.myValue];

             OptionsExtraClip.OptStat10.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat10.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat10.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat10.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             OptionsExtraClip.OptStat10.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             OptionsExtraClip.OptStat10.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             OptionsExtraClip.OptStat10.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_KillStreakSound, me.arCrossQuake, this); }
             OptionsExtraClip.OptStat10.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_KillStreakSound, me.arCrossQuake, this); }
			 
// CrosshaireColor			 
			OptionsExtraClip.OptStat11.Name.text = "$FR_optCrosshaireColor";			
			OptionsExtraClip.OptStat11.Value.visible = false;
			OptionsExtraClip.OptStat11.coloricon.gotoAndStop(Opt_CrosshaireColor.myValue + 1);
			OptionsExtraClip.OptStat11.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			OptionsExtraClip.OptStat11.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			OptionsExtraClip.OptStat11.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);			

			OptionsExtraClip.OptStat11.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			OptionsExtraClip.OptStat11.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			OptionsExtraClip.OptStat11.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			OptionsExtraClip.OptStat11.ArrowLeft.ActionFunction = function() 
				{ 
					me.onArrowClick(-1, me.Opt_CrosshaireColor, me.arCrossHaireColor, this);
				    OptionsExtraClip.OptStat11.coloricon.gotoAndStop(Opt_CrosshaireColor.myValue + 1);
				}
            OptionsExtraClip.OptStat11.ArrowRight.ActionFunction = function() 
				{ 
					me.onArrowClick(+1, me.Opt_CrosshaireColor, me.arCrossHaireColor, this);
					OptionsExtraClip.OptStat11.coloricon.gotoAndStop(Opt_CrosshaireColor.myValue + 1);
				}

			stage.addEventListener(MouseEvent.MOUSE_UP, barUpFn);

			processKillSreakSelection ();
			processIncreaseFPSSelection ();
		}
		
		private function barPressFn(evt:MouseEvent)
		{
			var	target = evt.currentTarget;
			var barW:Number = evt.currentTarget.width;
			var coord:Point = evt.currentTarget.globalToLocal(new Point(evt.stageX, evt.stageY));
			var xPos = (coord.x / barW);

			setSliderValue (evt.currentTarget.parent, xPos);
			isMouseDown = true;
			downTarget = target;
		}
		
		private function barMoveFn(evt:MouseEvent)
		{
			var	target = evt.currentTarget;
			
			if (isMouseDown && downTarget  == target)
			{
				var barW:Number = evt.currentTarget.width;
				var coord:Point = evt.currentTarget.globalToLocal(new Point(evt.stageX, evt.stageY));
				var xPos = (coord.x / barW);

				setSliderValue (evt.currentTarget.parent, xPos);
			}
		}
		
		private function barUpFn(evt:MouseEvent)
		{
			var	target = evt.currentTarget;
			
			isMouseDown = false;
			downTarget = null;
		}
		
		private function barPressFn2(evt:MouseEvent)
		{
			var	target = evt.currentTarget;
			var barW:Number = evt.currentTarget.width;
			var coord:Point = evt.currentTarget.globalToLocal(new Point(evt.stageX, evt.stageY));
			var xPos = 0.25 + (coord.x / barW) * 0.5;

			setSliderValue2 (evt.currentTarget.parent, xPos);
			isMouseDown = true;
			downTarget = target;
		}
		
		private function barMoveFn2(evt:MouseEvent)
		{
			var	target = evt.currentTarget;
			
			if (isMouseDown && downTarget  == target)
			{
				var barW:Number = evt.currentTarget.width;
				var coord:Point = evt.currentTarget.globalToLocal(new Point(evt.stageX, evt.stageY));
				var xPos = 0.25 + (coord.x / barW) * 0.5;

				setSliderValue2 (evt.currentTarget.parent, xPos);
			}
		}
		
		private function barUpFn2(evt:MouseEvent)
		{
			var	target = evt.currentTarget;
			
			isMouseDown = false;
			downTarget = null;
		}
		
		// param - from 0.0 to 1.0
		private function setSliderValue(control:MovieClip, param:Number)
		{
			param = Math.min (param, 1);
			param = Math.max (param, 0);	

			var barW:Number = 550;
			var	width:Number = param * barW;
			control.Bar.Scale.width = width;
		}
		
		private function getSliderValue(control:MovieClip):Number
		{
			var ret:Number = control.Bar.Scale.scaleX;
			if(ret<0) ret = 0;
			return ret;
		}
		
		// param - from 0.25 to 0.75
		private function setSliderValue2(control:MovieClip, param:Number)
		{
			param = (param - 0.25) / 0.5;
			param = Math.min (param, 1);
			param = Math.max (param, 0);	

			var barW:Number = 550;// control.Bar.width;
			var	width:Number = param * barW;
			control.Bar.Scale.width = width;
		}
		
		private function getSliderValue2(control:MovieClip):Number
		{
			var ret:Number = control.Bar.Scale.scaleX;
			
			ret = (ret * 0.5) + 0.25;
			ret = Math.min (ret, 0.75);
			ret = Math.max (ret, 0.25);	

			return ret;
		}
		
		private function processKillSreakSelection()
		{
			if(Opt_KillStreakSndOnOff.myValue == 1)
			{
				OptionsExtraClip.OptStat10.alpha = 1;
				OptionsExtraClip.OptStat10.State = "none";
				OptionsExtraClip.OptStat10.ArrowRight.useHandCursor = OptionsExtraClip.OptStat10.ArrowLeft.useHandCursor = true;
	
			}
			else
			{
				OptionsExtraClip.OptStat10.alpha = 0.5;
				OptionsExtraClip.OptStat10.State = "disabled";
				OptionsExtraClip.OptStat10.ArrowRight.useHandCursor = OptionsExtraClip.OptStat10.ArrowLeft.useHandCursor = false;	
			}
		}
		private function processIncreaseFPSSelection()
		{
			if(Opt_IncreaseFPS.myValue == 0)
			{
				OptionsExtraClip.OptStat5.alpha = 1;
				OptionsExtraClip.OptStat5.State = "none";
				OptionsExtraClip.OptStat5.ArrowRight.useHandCursor = OptionsExtraClip.OptStat5.ArrowLeft.useHandCursor = true;
	
			}
			else
			{
				OptionsExtraClip.OptStat5.alpha = 0.5;
				OptionsExtraClip.OptStat5.State = "disabled";
				OptionsExtraClip.OptStat5.ArrowRight.useHandCursor = OptionsExtraClip.OptStat5.ArrowLeft.useHandCursor = false;	
			}
		}		
	}
}
