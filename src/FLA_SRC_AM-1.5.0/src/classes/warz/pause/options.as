package warz.pause {
	import flash.display.MovieClip;
	import flash.events.Event;
	import flash.events.MouseEvent;
	import flash.geom.Point;
	import flash.geom.Rectangle;
	import flash.utils.clearInterval;
	import flash.utils.setInterval;
	import warz.events.PauseEvents;
	import warz.events.SoundEvents;
	import caurina.transitions.Tweener;

	public class options extends MovieClip {
		
		public var api:warz.pause.pause=null;
		
		public var Opt_OverallQ:Object;
		public var Opt_Brightness:Object;
		//public var Opt_Contrast:Object;
		public var Opt_SoundV:Object;
		public var Opt_MusicV:Object;
		public var Opt_CommV:Object;
		public var Opt_vertLook:Object;
		public var Opt_mouseWheel:Object;
		public var Opt_mouseSens:Object;
		public var Opt_mouseAccel:Object;
		
		public var Opt_Crosshair :Object; //Opt_Crosshair
		public var Opt_Highlight :Object; //Opt_Highlight
		public var Opt_ChatSound :Object; //ChatSound
		public var Opt_JumpSound :Object; //Opt_JumpSound		
		public var Opt_KillStreakSndOnOff :Object;//Opt_KillStreakSndOnOff		
		public var Opt_KillStreakSound :Object;//Opt_KillStreakSound
		public var Opt_CrosshaireColor :Object;//Opt_CrosshaireColor
		public var Opt_AlphaSound :Object;//Opt_AlphaSound

		public var arLMH :Array; // low, medium, high
		public var arLMHU :Array; // low, medium, high, ultra
		public var arLMHUC :Array; // low, medium high, ultra, custom
		public var arNY :Array; // No, Yes
		public var arLH :Array; // Low, High
		public var arOLMHU :Array; // Off, Low, Medium, High, Ultra
		public var arVLMHU :Array; // Very Low, Low, Medium, High, Ultra
		public var arOLMH :Array; // Off, Low, Medium, High
		
		public var arCrossHair :Array; // old, 1, 2, 3, 4, 5, 6, 7, 8
		public var arHighlight :Array; // old, 1, 2, 3, 4, 5, 6, 7
		public var arCrossQuake :Array; //CrossFire, Quake, Duke
		public var arCrossHaireColor :Array; //"white", "red", "blue", "green"

		public	var	isMouseDown:Boolean;
		public	var	downTarget:MovieClip;

		private var listenForKeyboardTarget : MovieClip;
		private var sendKeyRemapRequestInterval;
		
		public	var	keyControls:MovieClip;

		private var ScrollerIsDragging:Boolean;
		private var Scroller:MovieClip;

		public function options() {
			Opt_OverallQ = new Object();
			Opt_Brightness = new Object ();
			//Opt_Contrast = new Object();
			Opt_SoundV = new Object ();
			Opt_MusicV = new Object ();
			Opt_CommV = new Object ();
			Opt_vertLook = new Object ();
			Opt_mouseWheel = new Object ();
			Opt_mouseSens = new Object ();
			Opt_mouseAccel = new Object ();
			
			Opt_Crosshair = new Object(); //Opt_Crosshair
			Opt_Highlight = new Object(); //Opt_Highlight
			Opt_ChatSound = new Object(); //ChatSound
			Opt_JumpSound = new Object(); // Opt_JumpSound
			Opt_KillStreakSndOnOff = new Object(); // Opt_KillStreakSndOnOff
			Opt_KillStreakSound = new Object(); // Opt_KillStreakSound
			Opt_CrosshaireColor = new Object(); // Opt_CrosshaireColor
			Opt_CrosshaireColor = new Object(); // Opt_CrosshaireColor
			Opt_AlphaSound = new Object(); // Opt_AlphaSound

			arLMH = new Array("$FR_optLow", "$FR_optMed", "$FR_optHig");
			arLMHU = new Array("$FR_optLow", "$FR_optMed", "$FR_optHig", "$FR_optUlt");
			arLMHUC = new Array("$FR_optLow", "$FR_optMed", "$FR_optHig", "$FR_optUlt", "$FR_optCus");
			arNY = new Array("$FR_optNo", "$FR_optYes"); 
			arLH = new Array("$FR_optLow", "$FR_optHig");
			arOLMHU = new Array("$FR_optOff", "$FR_optLow", "$FR_optMed", "$FR_optHig", "$FR_optUlt");
			arVLMHU = new Array("$FR_optVeryLow", "$FR_optLow", "$FR_optMed", "$FR_optHig", "$FR_optUlt");
			arOLMH = new Array("$FR_optOff", "$FR_optLow", "$FR_optMed", "$FR_optHig");
			
			arCrossHair = new Array("1", "2", "3", "4", "5", "6", "7", "8");
			arHighlight = new Array("1", "2", "3", "4", "5", "6", "7");
			arCrossQuake = new Array("$FR_optCross", "$FR_optQuake", "$FR_optDuke");
			arCrossHaireColor = new Array("0", "1", "2", "3");

			ResetBtn.Btn.addEventListener(MouseEvent.MOUSE_OVER, ButtonRollOverFn);
			ResetBtn.Btn.addEventListener(MouseEvent.MOUSE_OUT, ButtonRollOutFn);
			ResetBtn.Btn.addEventListener(MouseEvent.CLICK, ButtonPressFn);
			ResetBtn.Text.Text.text = "$FR_Reset";

			ApplyBtn.Btn.addEventListener(MouseEvent.MOUSE_OVER, ButtonRollOverFn);
			ApplyBtn.Btn.addEventListener(MouseEvent.MOUSE_OUT, ButtonRollOutFn);
			ApplyBtn.Btn.addEventListener(MouseEvent.CLICK, ButtonPressFn);
			ApplyBtn.Text.Text.text = "$FR_Apply";			
			
			keyControls = new MovieClip ();
			keyControls.mask = this.Mask;				
			this.addChild(keyControls);

			var	me = this;
			
			Scroller = KeyScroller;
			ScrollerIsDragging = false;
			Scroller.Field.alpha = 0.5;
			Scroller.ArrowUp.alpha = 0.5;
			Scroller.ArrowDown.alpha = 0.5;
			Scroller.Trigger.alpha = 0.5;

			Scroller.Trigger.addEventListener(MouseEvent.MOUSE_OVER, function() {if(me.ScrollerIsDragging==true) return; me.Scroller.Trigger.alpha = 1; })
			Scroller.Trigger.addEventListener(MouseEvent.MOUSE_OUT, function() {if(me.ScrollerIsDragging==true) return; me.Scroller.Trigger.alpha = 0.5; })
			Scroller.ArrowUp.addEventListener(MouseEvent.MOUSE_OVER, function() {me.Scroller.ArrowUp.alpha = 1;})
			Scroller.ArrowUp.addEventListener(MouseEvent.MOUSE_OUT, function() {me.Scroller.ArrowUp.alpha = 0.5;})
			Scroller.ArrowUp.addEventListener(MouseEvent.CLICK, function() {me.handleScrollerUpDown(-10);})
			Scroller.ArrowDown.addEventListener(MouseEvent.MOUSE_OVER, function() {me.Scroller.ArrowDown.alpha = 1;})
			Scroller.ArrowDown.addEventListener(MouseEvent.MOUSE_OUT, function() {me.Scroller.ArrowDown.alpha = 0.5; })
			Scroller.ArrowDown.addEventListener(MouseEvent.CLICK, function() {me.handleScrollerUpDown(+10);})
			
			Scroller.Trigger.addEventListener(MouseEvent.MOUSE_DOWN, function() 
			{
				var startY = me.Scroller.Field.y;
				var endY = me.Scroller.Field.height - me.Scroller.Trigger.height;
				me.ScrollerIsDragging = true;
				me.Scroller.Trigger.startDrag(false, new Rectangle (me.Scroller.Trigger.x, startY, 0, endY));
			})
		}
		
		public function Activate()
		{
			api = warz.pause.pause.api;

			visible = true;
			
			PauseEvents.eventDisableHotKeys(false);
			
			resetValues ();
			updateKeyMapping ();
			
			stage.addEventListener(MouseEvent.MOUSE_WHEEL, scrollerMouseWheel);
			stage.addEventListener(MouseEvent.MOUSE_UP, mouseUp);
		}
		
		public function Deactivate()
		{
			visible = false;
			
			stage.removeEventListener(MouseEvent.MOUSE_WHEEL, scrollerMouseWheel);
			stage.removeEventListener(MouseEvent.MOUSE_UP, mouseUp);
		}
		
		public	function mouseUp(e:Event) :void
		{ 
			ScrollerIsDragging = false;
			Scroller.Trigger.stopDrag(); 
		}
		
		public	function scrollerMouseWheel (e:MouseEvent):void
		{
			if (e.stageX > this.Mask.x && 
				e.stageX < this.Mask.x + this.Mask.width + 45 &&
				e.stageY > this.Mask.y && 
				e.stageY < this.Mask.y + this.Mask.height)
			{
				var dist = (keyControls.height - this.Mask.height) / 51;
				var h = Scroller.Field.height - Scroller.Trigger.height + 26;
				dist = h / dist;

				if (e.delta > 0)
					handleScrollerUpDown(-dist);
				else
					handleScrollerUpDown(dist);
			}
		}
		
		public	function resetValues ()
		{
			var	me = this;
			
			Opt_OverallQ.myValue = api.Opt_OverallQ-1;
			Opt_Brightness.myValue = api.Opt_Brightness;
			//Opt_Contrast.myValue = api.Opt_Contrast;
			Opt_SoundV.myValue = api.Opt_SoundV;
			Opt_MusicV.myValue = api.Opt_MusicV;
			Opt_CommV.myValue = api.Opt_CommV;
			Opt_vertLook.myValue = api.Opt_vertLook;
			Opt_mouseWheel.myValue = api.Opt_mouseWheel;
			Opt_mouseSens.myValue = api.Opt_mouseSens;
			Opt_mouseAccel.myValue = api.Opt_mouseAccel;
			
			Opt_Crosshair.myValue = api.Opt_Crosshair-1; // Opt_Crosshair
			Opt_Highlight.myValue = api.Opt_Highlight-1; // Opt_Highlight
			Opt_ChatSound.myValue = api.Opt_ChatSound-1; //ChatSound
			Opt_JumpSound.myValue = api.Opt_JumpSound-1; // Opt_JumpSound
			Opt_KillStreakSndOnOff.myValue = api.Opt_KillStreakSndOnOff-1; // Opt_KillStreakSndOnOff			
			Opt_KillStreakSound.myValue = api.Opt_KillStreakSound-1; // Opt_KillStreakSound
			Opt_CrosshaireColor.myValue = api.Opt_CrosshaireColor-1; // Opt_CrosshaireColor
			Opt_AlphaSound.myValue = api.Opt_AlphaSound-1; // Opt_AlphaSound

// quality
//			OptStat2
			OverallQ.Name.text = "$FR_optOverallQuality";
			OverallQ.Value.Text.text = arLMHUC[Opt_OverallQ.myValue];

			OverallQ.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			OverallQ.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			OverallQ.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			OverallQ.ArrowLeft.ActionFunction = function ()
			{
				me.onArrowClick(-1, me.Opt_OverallQ, me.arLMHUC, this);
			}			

			OverallQ.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			OverallQ.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			OverallQ.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			OverallQ.ArrowRight.ActionFunction = function ()
			{
				me.onArrowClick(+1, me.Opt_OverallQ, me.arLMHUC, this);
			}
			
// brightness
			Brightness.Name.text = "$FR_optBrightness";
			Brightness.Bar.Scale.scaleX = 0.99; // api.Opt_Brightness;
			Brightness.Bar.addEventListener(MouseEvent.MOUSE_DOWN, barPressFn2);
			Brightness.Bar.addEventListener(MouseEvent.MOUSE_MOVE, barMoveFn2);
			Brightness.Bar.addEventListener(MouseEvent.MOUSE_UP, barUpFn2);

			setSliderValue2(Brightness, Opt_Brightness.myValue);
			
// contrast
			//Contrast.Name.text = "$FR_optContrast";
			//Contrast.Bar.Scale.scaleX = 0.99; // api.Opt_Contrast;
			//Contrast.Bar.addEventListener(MouseEvent.MOUSE_DOWN, barPressFn2);
			//Contrast.Bar.addEventListener(MouseEvent.MOUSE_MOVE, barMoveFn2);
			//Contrast.Bar.addEventListener(MouseEvent.MOUSE_UP, barUpFn2);

			//setSliderValue2(Contrast, Opt_Contrast.myValue);
			
// sound Volume
			Volume.Name.text = "$FR_optSoundVolume";
			Volume.Bar.Scale.scaleX = 0.99;
			Volume.Bar.addEventListener(MouseEvent.MOUSE_DOWN, barPressFn);
			Volume.Bar.addEventListener(MouseEvent.MOUSE_MOVE, barMoveFn);
			Volume.Bar.addEventListener(MouseEvent.MOUSE_UP, barUpFn);

			setSliderValue(Volume, Opt_SoundV.myValue);

// Music Volume
			MusicVolume.Name.text = "$FR_optMusicVolume";
			MusicVolume.Bar.Scale.scaleX = 0.99; 
			MusicVolume.Bar.addEventListener(MouseEvent.MOUSE_DOWN, barPressFn);
			MusicVolume.Bar.addEventListener(MouseEvent.MOUSE_MOVE, barMoveFn);
			MusicVolume.Bar.addEventListener(MouseEvent.MOUSE_UP, barUpFn);

			setSliderValue(MusicVolume, Opt_MusicV.myValue);

// Comms Volume
			SpeechVolume.Name.text = "$FR_optVoipVoiceChatVolume";
			SpeechVolume.Bar.Scale.scaleX = 0.99;
			SpeechVolume.Bar.addEventListener(MouseEvent.MOUSE_DOWN, barPressFn);
			SpeechVolume.Bar.addEventListener(MouseEvent.MOUSE_MOVE, barMoveFn);
			SpeechVolume.Bar.addEventListener(MouseEvent.MOUSE_UP, barUpFn);

			setSliderValue(SpeechVolume, Opt_CommV.myValue);

// invert Y
			InvertY.Name.text = "$FR_optInvertYAxis";
			InvertY.Value.Text.text = arNY[Opt_vertLook.myValue];
			InvertY.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_vertLook, me.arNY, this); }
			InvertY.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_vertLook, me.arNY, this); }
			InvertY.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			InvertY.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			InvertY.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			InvertY.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			InvertY.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			InvertY.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);

			MouseWheel.Name.text = "$FR_optMouseWheel";
			MouseWheel.Value.Text.text = arNY[Opt_mouseWheel.myValue];
			MouseWheel.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_mouseWheel, me.arNY, this); }
			MouseWheel.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_mouseWheel, me.arNY, this); }
			MouseWheel.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			MouseWheel.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			MouseWheel.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			MouseWheel.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			MouseWheel.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			MouseWheel.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			Sensitivity.Name.text = "$FR_optSensitivity";
			Sensitivity.Bar.Scale.scaleX = 0.99;
			Sensitivity.Bar.addEventListener(MouseEvent.MOUSE_DOWN, barPressFn3);
			Sensitivity.Bar.addEventListener(MouseEvent.MOUSE_MOVE, barMoveFn3);
			Sensitivity.Bar.addEventListener(MouseEvent.MOUSE_UP, barUpFn3);

			setSliderValue3(Sensitivity, Opt_mouseSens.myValue);
			
			Run.Name.text = "$FR_optAcceleration";
			Run.Value.Text.text = arNY[Opt_mouseAccel.myValue];
			Run.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_mouseAccel, me.arNY, this); }
			Run.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_mouseAccel, me.arNY, this); }
			Run.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			Run.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			Run.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			Run.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			Run.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			Run.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
// Crosshair new option
			CrosshairType.Name.text = "$FR_optCrosshair";			
			CrosshairType.Value.visible = false;
			CrosshairType.reticle.gotoAndStop(Opt_Crosshair.myValue + 1);
			CrosshairType.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			CrosshairType.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			CrosshairType.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);			

			CrosshairType.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			CrosshairType.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			CrosshairType.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			CrosshairType.ArrowLeft.ActionFunction = function() 
				{ 
					me.onArrowClick(-1, me.Opt_Crosshair, me.arCrossHair, this);
				    CrosshairType.reticle.gotoAndStop(Opt_Crosshair.myValue + 1);
				}
            CrosshairType.ArrowRight.ActionFunction = function() 
				{ 
					me.onArrowClick(+1, me.Opt_Crosshair, me.arCrossHair, this);
					CrosshairType.reticle.gotoAndStop(Opt_Crosshair.myValue + 1);
				}
			
// Highlight new option
			LootColor.Name.text = "$FR_optHighlight";			
			LootColor.Value.visible = false;
			LootColor.coloricon.gotoAndStop(Opt_Highlight.myValue + 1);
			LootColor.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			LootColor.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			LootColor.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);			

			LootColor.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			LootColor.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			LootColor.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			LootColor.ArrowLeft.ActionFunction = function() 
				{ 
					me.onArrowClick(-1, me.Opt_Highlight, me.arHighlight, this);
				    LootColor.coloricon.gotoAndStop(Opt_Highlight.myValue + 1);
				}
            LootColor.ArrowRight.ActionFunction = function() 
				{ 
					me.onArrowClick(+1, me.Opt_Highlight, me.arHighlight, this);
					LootColor.coloricon.gotoAndStop(Opt_Highlight.myValue + 1);
				}

// ChatSound	
			 ChatSound.Name.text = "$FR_optChatSound";
             ChatSound.Value.Text.text = arNY[Opt_ChatSound.myValue];
			// KillStreakSound.Value.gotoAndPlay("start");
             ChatSound.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             ChatSound.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             ChatSound.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
             ChatSound.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             ChatSound.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             ChatSound.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
             ChatSound.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_ChatSound, me.arNY, this); }
             ChatSound.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_ChatSound, me.arNY, this); } 
			 
// JumpSound 
			 JumpSound.Name.text = "$FR_optJumpSound";
             JumpSound.Value.Text.text = arNY[Opt_JumpSound.myValue];

             JumpSound.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             JumpSound.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             JumpSound.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             JumpSound.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             JumpSound.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             JumpSound.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             JumpSound.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_JumpSound, me.arNY, this); }
             JumpSound.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_JumpSound, me.arNY, this); }	
			 
//KillStreakSndOnOff
			 KillStreakSndOnOff.Name.text = "$FR_optKillStreakSndOnOff";
             KillStreakSndOnOff.Value.Text.text = arNY[Opt_KillStreakSndOnOff.myValue];

             KillStreakSndOnOff.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             KillStreakSndOnOff.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             KillStreakSndOnOff.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             KillStreakSndOnOff.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             KillStreakSndOnOff.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             KillStreakSndOnOff.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             KillStreakSndOnOff.ArrowLeft.ActionFunction = function() 
				{ 
					me.onArrowClick(-1, me.Opt_KillStreakSndOnOff, me.arNY, this); 
					processKillStreakSelection ();
				}
             KillStreakSndOnOff.ArrowRight.ActionFunction = function() 
				{ 
					me.onArrowClick(+1, me.Opt_KillStreakSndOnOff, me.arNY, this); 
					processKillStreakSelection ();
				}

// KillStreakSound 
			 KillStreakSound.Name.text = "$FR_optKillStreakSound";
             KillStreakSound.Value.Text.text = arCrossQuake[Opt_KillStreakSound.myValue];

             KillStreakSound.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             KillStreakSound.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             KillStreakSound.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             KillStreakSound.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             KillStreakSound.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             KillStreakSound.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             KillStreakSound.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_KillStreakSound, me.arCrossQuake, this); }
             KillStreakSound.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_KillStreakSound, me.arCrossQuake, this); }	

// CrosshaireColor
//		OptStat33 			 
			CrosshaireColor.Name.text = "$FR_optCrosshaireColor";			
			CrosshaireColor.Value.visible = false;
			CrosshaireColor.coloricon.gotoAndStop(Opt_CrosshaireColor.myValue + 1);
			CrosshaireColor.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			CrosshaireColor.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			CrosshaireColor.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);			

			CrosshaireColor.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			CrosshaireColor.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			CrosshaireColor.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			CrosshaireColor.ArrowLeft.ActionFunction = function() 
				{ 
					me.onArrowClick(-1, me.Opt_CrosshaireColor, me.arCrossHaireColor, this);
				    CrosshaireColor.coloricon.gotoAndStop(Opt_CrosshaireColor.myValue + 1);
				}
            CrosshaireColor.ArrowRight.ActionFunction = function() 
				{ 
					me.onArrowClick(+1, me.Opt_CrosshaireColor, me.arCrossHaireColor, this);
					CrosshaireColor.coloricon.gotoAndStop(Opt_CrosshaireColor.myValue + 1);
				}

// AlphaSound	 
			 AlphaSounds.Name.text = "$FR_optAlphaSound";
             AlphaSounds.Value.Text.text = arNY[Opt_AlphaSound.myValue];

             AlphaSounds.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             AlphaSounds.ArrowLeft.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             AlphaSounds.ArrowLeft.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             AlphaSounds.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
             AlphaSounds.ArrowRight.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
             AlphaSounds.ArrowRight.Btn.addEventListener(MouseEvent.CLICK, arrowPressFn);
			 
             AlphaSounds.ArrowLeft.ActionFunction = function() { me.onArrowClick(-1, me.Opt_AlphaSound, me.arNY, this); }
             AlphaSounds.ArrowRight.ActionFunction = function() { me.onArrowClick(+1, me.Opt_AlphaSound, me.arNY, this); }				
			
			stage.addEventListener(MouseEvent.MOUSE_UP, barUpFn);
		}
		
		private function processKillStreakSelection()
		{
			if(Opt_KillStreakSndOnOff.myValue == 1)
			{
				KillStreakSound.alpha = 1;
				KillStreakSound.State = "none";
				KillStreakSound.ArrowRight.useHandCursor = KillStreakSound.ArrowLeft.useHandCursor = true;
	
			}
			else
			{
				KillStreakSound.alpha = 0.5;
				KillStreakSound.State = "disabled";
				KillStreakSound.ArrowRight.useHandCursor = KillStreakSound.ArrowLeft.useHandCursor = false;	
			}
		}
		
		private function barPressFn(evt:MouseEvent)
		{
			SoundEvents.eventSoundPlay("menu_click");
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
			SoundEvents.eventSoundPlay("menu_click");
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
		
		private function barPressFn3(evt:MouseEvent)
		{
			SoundEvents.eventSoundPlay("menu_click");
			var barW:Number = evt.currentTarget.width;
			var coord:Point = evt.currentTarget.globalToLocal(new Point(evt.stageX, evt.stageY));
			var xPos = (coord.x / barW) * 10;

			setSliderValue3 (evt.currentTarget.parent, xPos);
			isMouseDown = true;
		}
		
		private function barMoveFn3(evt:MouseEvent)
		{
			if (isMouseDown)
			{
				var barW:Number = evt.currentTarget.width;
				var coord:Point = evt.currentTarget.globalToLocal(new Point(evt.stageX, evt.stageY));
				var xPos = (coord.x / barW) * 10;
	
				setSliderValue3 (evt.currentTarget.parent, xPos);
			}
		}
		
		private function barUpFn3(evt:MouseEvent)
		{
			var	target = evt.currentTarget;
			
			isMouseDown = false;
			downTarget = null;
		}
		
		private function setSliderValue3(control:MovieClip, param:Number)
		{
			var barW:Number = 550;
			
			param = Math.min (param, 10);
			param = Math.max (param, 0);	

			var	width:Number = param / 10 * barW;
			control.Bar.Scale.width = width;
			
			var	str:String = (param).toString(10);
			if(str.length > 4)
				str = str.slice(0, 4);

//			control.Value.Text.text = str;
		}
		
		private function getSliderValue3(control:MovieClip):Number
		{
			var ret:Number = control.Bar.Scale.width / control.Bar.width;
			return ret*10;
		}

		public	function ButtonRollOverFn(evt:Event)
		{
			if(evt.currentTarget.parent.State != "active")
				evt.currentTarget.parent.gotoAndPlay("over");
				
			SoundEvents.eventSoundPlay("menu_hover");
		}
		
		public	function ButtonRollOutFn(evt:Event)
		{
			if(evt.currentTarget.parent.State != "active")
				evt.currentTarget.parent.gotoAndPlay("out");
		}
		
		public	function ButtonPressFn(evt:Event)
		{
			if(evt.currentTarget.parent.State != "active")
			{
				evt.currentTarget.parent.State = "active";
				evt.currentTarget.parent.gotoAndPlay("pressed");
				
				var	p:MovieClip = evt.currentTarget.parent;
				
				while (!(p as options))
				{
					p = p.parent as MovieClip;
				}
				
				SoundEvents.eventSoundPlay("menu_click");
				
				(p as options).ActionFunction(evt.currentTarget.parent.name);
			}
		}		
		
		public	function ActionFunction (button:String)
		{
			if (button == "ResetBtn")
			{
				ResetBtn.State = "off";
				ResetBtn.gotoAndPlay("out");
				api.showInfoOkCancelMsg("$FR_ResetSettingAreYouSure", "", ResetBtnCallBack); 
			}
			else if (button == "ApplyBtn")
			{
				ApplyBtn.State = "off";
				ApplyBtn.gotoAndPlay("out");	
				api.showInfoOkCancelMsg("$FR_ApplySettingAreYouSure","", ApplyBtnCallBack); 
			}
		}
		
		public function ResetBtnCallBack(isReset:Boolean):void{
			if(isReset){
				PauseEvents.eventOptionsControlsReset();
				PauseEvents.eventBackToGame();
			}
		}
		
		public function ApplyBtnCallBack(isApply:Boolean):void{
			if(isApply){
				Opt_mouseSens.myValue = getSliderValue3(Sensitivity);
				Opt_Brightness.myValue = getSliderValue2(Brightness);
				//Opt_Contrast.myValue = getSliderValue2(Contrast);
					
				Opt_SoundV.myValue = getSliderValue(Volume);
				Opt_MusicV.myValue = getSliderValue(MusicVolume);
				Opt_CommV.myValue = getSliderValue(SpeechVolume);
					
				PauseEvents.eventOptionsApply(Opt_mouseSens.myValue, Opt_Brightness.myValue, 0,//Opt_Contrast.myValue, 
											Opt_SoundV.myValue, Opt_MusicV.myValue, Opt_CommV.myValue,
											Opt_OverallQ.myValue + 1, 0, Opt_vertLook.myValue,
											Opt_mouseWheel.myValue, Opt_mouseAccel.myValue, 0, Opt_Crosshair.myValue+1,
											Opt_Highlight.myValue+1, Opt_ChatSound.myValue+1, Opt_JumpSound.myValue+1, Opt_KillStreakSndOnOff.myValue+1,
											Opt_KillStreakSound.myValue+1, Opt_CrosshaireColor.myValue+1, Opt_AlphaSound.myValue+1); 
				PauseEvents.eventBackToGame();
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
				
			SoundEvents.eventSoundPlay("menu_click");
				
			var myVar:Number = variable.myValue;
			var prevMode:Number = myVar;
			myVar += incr;
			if(myVar<0)
				myVar = myArray.length-1;
			if(myVar>=myArray.length)
				myVar = 0;
	
			p.Value.Text.text = myArray[myVar];
			variable.myValue = myVar;
		}
				
		private function arrowPressFn(evt:Event)
		{
			if(evt.currentTarget.parent.State != "active")
			{
				SoundEvents.eventSoundPlay("menu_click");
				evt.currentTarget.parent.ActionFunction ();
			}
		}		
		
		public	function updateKeyMapping ()
		{
			var me = this;
			listenForKeyboardTarget = null;
//			KeyboardControls.BaseControl._visible = false;
			
			for(var i = 0; i < 8; i++)
			{
				var NewName = "Control"+ String (i + 1);
				
				if(this[NewName])
					this[NewName].visible = false;
			}
			
			while (keyControls.numChildren > 0)
			{
				keyControls.removeChildAt(0);
			}
			
			for(i=0;i<api.keyboardMapping.length;i++)
			{
				var keyControl = new warz.pause.KeyControl();
				keyControls.addChild(keyControl);
				
				keyControl.x = this["Control1"].x;
				keyControl.y = this["Control1"].y + 51 * i;
				
				keyControl.addEventListener(MouseEvent.MOUSE_OVER, function (evt:MouseEvent) {
					if (me.listenForKeyboardTarget == null) {
						evt.currentTarget.gotoAndPlay("over");
					}
				});
				keyControl.addEventListener(MouseEvent.MOUSE_OUT, function (evt:MouseEvent) {
					if (me.listenForKeyboardTarget == null) {
						evt.currentTarget.gotoAndPlay("out");
					}
				});
				keyControl.addEventListener(MouseEvent.CLICK, function (evt:MouseEvent) {
					if (me.listenForKeyboardTarget == null) {
						me.listenForKeyboardTarget = evt.currentTarget;
						evt.currentTarget.gotoAndPlay("pressed");
						clearInterval(me.sendKeyRemapRequestInterval);
						me.sendKeyRemapRequestInterval = setInterval (sendKeyRemapRequest, 100);
					}
				});

				keyControl.Name.Name.text = api.keyboardMapping[i]["name"];
				keyControl.Value.Text.text = api.keyboardMapping[i]["key"];
				keyControl.remapIndex = i;
			}
			
			if (api.keyboardMapping.length > 11)
			{
				Scroller.visible = true;
				
				//Scroller.Trigger.y = Scroller.Field.y;//cynthia: 53 
				Scroller.Trigger.addEventListener(Event.ENTER_FRAME, scrollItems);

				Scroller.ArrowDown.y = this.Mask.height - 4;
				Scroller.Field.height = (this.Mask.height - (Scroller.ArrowDown.height * 2));
			}
			else
			{
				Scroller.Trigger.removeEventListener(Event.ENTER_FRAME, scrollItems);
				Scroller.visible = false;
				keyControls.y = 0;
			}
		}

		public	function scrollItems(e:Event)
		{
			var	a = Scroller.Field.y;
			var	b = Scroller.Trigger.y;
			var	dist = (Scroller.Field.y - Scroller.Trigger.y);
			var	h = keyControls.height;
			var	h1 = Scroller.Field.height - Scroller.Trigger.height;
			var	mh = this.Mask.height - 5;
			h -= mh;
			h /= h1;
		
			var	dest:Number = dist * h;
			var	step:Number = int (dest / 51);
			
			if (keyControls.y != 51 * step)
			{
				Tweener.addTween(keyControls, {y:51 * step, time:0.25, transition:"linear"});
			}
		
//			keyControls.y = dist * h;
		}

		public function sendKeyRemapRequest()
		{
			clearInterval(sendKeyRemapRequestInterval);
			PauseEvents.eventOptionsControlsRequestKeyRemap(listenForKeyboardTarget.remapIndex);
		}
		
		public function handleScrollerUpDown(delta:Number)
		{
			if(delta < 0)
			{ 
				var startY = Scroller.Field.y; 
				if(Scroller.Trigger.y > startY) { 
					Scroller.Trigger.y += delta; 
					if(Scroller.Trigger.y < startY) 
						Scroller.Trigger.y = startY; 
				} 
			}
			else
			{
				var endY = Scroller.Field.height - Scroller.Trigger.height+26;
				if(Scroller.Trigger.y < endY) { 
					Scroller.Trigger.y += delta; 
					if(Scroller.Trigger.y > endY) 
						Scroller.Trigger.y = endY; 
				} 
			}
		}
	}
}
