package warz.frontend {
	import flash.display.MovieClip;
	import flash.events.*;
	import warz.events.FrontEndEvents;
	import warz.events.SoundEvents;
	import warz.dataObjects.*;
	import flash.geom.Rectangle;
	import flash.display.Bitmap;
	import warz.utils.ImageLoader;
	import warz.utils.Layout;
	import caurina.transitions.Tweener;
	import warz.Utils;
	
	public class ChangeSurvivorScreen extends MovieClip 
	{
		public var CharSlots:MovieClip;
        public var CharacterMask:MovieClip;        
        public var Scroller:MovieClip;		
		public var isActiveScreen:Boolean = false;
		private var ScrollerIsDragging:Boolean;
		public var heroSlots:MovieClip;		
		public  var BtnBack:MovieClip;
		public  var BtnCreate:MovieClip;
		public 	var Character:MovieClip;
		
		public 	var api:warz.frontend.Frontend=null;
		
		public	var	curHero:Hero;
		public	var	body:int;
		public	var	hair:int;
		public	var	head:int;
		public	var	legs:int;
		
		public function ChangeSurvivorScreen() 
		{
			this.visible = false;
			BtnBack.Btn.addEventListener(MouseEvent.MOUSE_OVER, SlotRollOverFn);
			BtnBack.Btn.addEventListener(MouseEvent.MOUSE_OUT, SlotRollOutFn);
			BtnBack.Btn.addEventListener(MouseEvent.CLICK, SlotPressFn);

			BtnCreate.Btn.addEventListener(MouseEvent.MOUSE_OVER, SlotRollOverFn);
			BtnCreate.Btn.addEventListener(MouseEvent.MOUSE_OUT, SlotRollOutFn);
			BtnCreate.Btn.addEventListener(MouseEvent.CLICK, SlotPressFn);
			
			BtnBack.Text.Text.text = "$FR_Back";
			BtnCreate.Text.Text.text = "$FR_Apply";

			Character.ArrowRightZoneBtn1.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			Character.ArrowRightZoneBtn1.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			Character.ArrowRightZoneBtn1.addEventListener(MouseEvent.CLICK, arrowPressFn);

			Character.ArrowRightZoneBtn2.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			Character.ArrowRightZoneBtn2.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			Character.ArrowRightZoneBtn2.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			Character.ArrowRightZoneBtn3.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			Character.ArrowRightZoneBtn3.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			Character.ArrowRightZoneBtn3.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			/// hair bottom
			Character.ArrowRightZoneBtn4.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			Character.ArrowRightZoneBtn4.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			Character.ArrowRightZoneBtn4.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			Character.ArrowLeftZoneBtn1.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			Character.ArrowLeftZoneBtn1.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			Character.ArrowLeftZoneBtn1.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			Character.ArrowLeftZoneBtn2.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			Character.ArrowLeftZoneBtn2.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			Character.ArrowLeftZoneBtn2.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			Character.ArrowLeftZoneBtn3.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			Character.ArrowLeftZoneBtn3.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			Character.ArrowLeftZoneBtn3.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			/// hair bottom
			Character.ArrowLeftZoneBtn4.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OVER, arrowRollOverFn);
			Character.ArrowLeftZoneBtn4.Arrow.Btn.addEventListener(MouseEvent.MOUSE_OUT, arrowRollOutFn);
			Character.ArrowLeftZoneBtn4.addEventListener(MouseEvent.CLICK, arrowPressFn);
			
			Character.TitleHair.text = "$FR_CREATE_SURVIVOR_Hair";
			Character.TitleHead.text = "$FR_CREATE_SURVIVOR_HEAD";
			Character.TitleTorso.text = "$FR_CREATE_SURVIVOR_UPPER_BODY";
			Character.TitleLegs.text = "$FR_CREATE_SURVIVOR_LEGS";
			
			heroSlots = CharSlots;			
			var me = this;			
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
			
			Scroller.Trigger.addEventListener(MouseEvent.MOUSE_DOWN, function(evt:MouseEvent) 
			{
				if(evt.buttonDown){
					var startY = me.Scroller.Field.y;
					var endY = me.Scroller.Field.height - me.Scroller.Trigger.height;
					me.ScrollerIsDragging = true;
					me.Scroller.Trigger.startDrag(false, new Rectangle (me.Scroller.Trigger.x, startY, 0, endY));
				}
			})
		}

		// event functions
		public	function SlotRollOverFn(evt:Event)
		{
			if(evt.currentTarget.parent.State != "active")
				evt.currentTarget.parent.gotoAndPlay("over");
		}
		
		public	function SlotRollOutFn(evt:Event)
		{
			if(evt.currentTarget.parent.State != "active")
				evt.currentTarget.parent.gotoAndPlay("out");
		}
		
		public	function scrollItems(e:Event)
		{
			var	a = Scroller.Field.y;
			var	b = Scroller.Trigger.y;
			var	dist = (Scroller.Field.y - Scroller.Trigger.y);
			var	h = heroSlots.height;
			var	h1 = Scroller.Field.height - Scroller.Trigger.height;
			var	mh = CharacterMask.height;
			h -= mh;
			h /= h1;

			var	dest:Number = dist * h;
			var	step:Number = int (dest / 236);
			
			if (heroSlots.y != (222.95+236*step))
			{
				Tweener.addTween(heroSlots, {y:222.95+(236 * step), time:api.tweenDelay, transition:"linear"});
			}

//			heroSlots.y = dist * h;
		}
		
		public	function heroRollOverFn(evt:Event)
		{
			if (evt.currentTarget.parent.Active == true)
				return;
				
			if (evt.currentTarget.parent.Inventory)
			{
				if (evt.currentTarget.parent.currentLabel != "over")
					evt.currentTarget.parent.gotoAndPlay("over");
			}
			else
			{
				if (evt.currentTarget.parent.currentLabel != "locked_over")
					evt.currentTarget.parent.gotoAndPlay("locked_over");
			}				
		}
		
		public	function heroRollOutFn(evt:Event)
		{
			if (evt.currentTarget.parent.Active == true)
				return;
				
			if (evt.currentTarget.parent.Inventory)
			{
				if (evt.currentTarget.parent.currentLabel != "out")
					evt.currentTarget.parent.gotoAndPlay("out");
			}
			else
			{
				if (evt.currentTarget.parent.currentLabel != "locked_out")
					evt.currentTarget.parent.gotoAndPlay("locked_out");
			}				
		}
		
		public function loadSlotIcon(path:String, imageHolder:MovieClip, slotType:Number)
		{
			var	dat:Object = {imageHolder:imageHolder, slotType:slotType};
			var	imageLoader:ImageLoader = new ImageLoader (path, loadSlotCallback, dat)
		}
		
		public	function loadSlotCallback (bitmap:Bitmap, dat:Object)
		{
			var slotWidth=0, slotHeight=0;
			var slotX=0, slotY=0;
			
			slotWidth = 256;
			slotHeight = 256;
			slotX = 0;
			slotY = 0;
			
			if(dat.slotType == 2)
				slotHeight = 128;

			bitmap = Layout.stretch (bitmap, slotWidth, slotHeight, bitmap.width, bitmap.height, "uniform");
			
			bitmap.x = slotX; 
			bitmap.y = slotY; 
		
			dat.imageHolder.addChild (bitmap);
		}
		
		public	function unlockRollOverFn(evt:Event)
		{
			if (evt.currentTarget.parent.Active == true)
				return;
				
			if (evt.currentTarget.currentLabel != "over")
				evt.currentTarget.gotoAndPlay("over");
		}
		
		public	function unlockRollOutFn(evt:Event)
		{
			if (evt.currentTarget.parent.Active == true)
				return;
				
			if (evt.currentTarget.currentLabel != "out")
				evt.currentTarget.gotoAndPlay("out");
		}
		
		public	function heroRollClickFn(evt:Event)
		{
			if(curHero == evt.currentTarget.parent.hero)
				return;
			
			for (var a:int = 0; a < heroSlots.numChildren; a++)
			{
				var	heroslot = heroSlots.getChildAt(a);
				
				heroslot.BtnUnlock.visible = false;
				heroslot.BtnInfo.visible = false;
				if (heroslot.Inventory)
				{
					if (heroslot.currentLabel != "out")
						heroslot.gotoAndPlay("out");
				}
				else
				{
					if (heroslot.currentLabel != "locked_out")
						heroslot.gotoAndPlay("locked_out");
				}			
				
				heroslot.Active = false;
			}			

			curHero = evt.currentTarget.parent.hero;
			body = 0;
			hair = 0;
			head = 0;
			legs = 0;
			UpdateArrowStatus();

			if (api.getInventoryItemByID (curHero.itemID))
			{				
				BtnCreate.gotoAndStop(1);				
				BtnCreate.State = "off";

			}
			else
			{				
				BtnCreate.gotoAndStop("inactive");			
				BtnCreate.State = "active";
			}
			
			FrontEndEvents.eventCreateChangeCharacter (curHero.itemID, hair, head, body, legs);

			if (evt.currentTarget.parent.Inventory)
			{
				if (evt.currentTarget.parent.currentLabel != "pressed")
					evt.currentTarget.parent.gotoAndPlay("pressed");
			}
			else
			{
				if (evt.currentTarget.parent.currentLabel != "locked_pressed")
					evt.currentTarget.parent.gotoAndPlay("locked_pressed");
			}			
			
			evt.currentTarget.parent.BtnUnlock.visible = true;//need fix(bug)
			evt.currentTarget.parent.BtnInfo.visible = true;
			
			evt.currentTarget.parent.Active = true;
		}
		
		public	function unlockRollClickFn(evt:Event)
		{
			if(curHero != evt.currentTarget.parent.hero)
				return;
			if (evt.currentTarget.parent.Active != true)
				return;

			if (evt.currentTarget.currentLabel != "pressed")
				evt.currentTarget.gotoAndPlay("pressed");
			
			SoundEvents.eventSoundPlay("menu_click");

			api.showCharacterUnlock (curHero.itemID);
			
		}
		
		public function updateHeros ()
		{
			
			if (api.HeroDB.length > 9)
			{
				Scroller.visible = true;
				var me = this;
				
				Scroller.Trigger.y = Scroller.Field.y;
				Scroller.Trigger.addEventListener(Event.ENTER_FRAME, scrollItems);
				
				Scroller.ArrowDown.y = CharacterMask.height - 4;
				Scroller.Field.height = (CharacterMask.height - (Scroller.ArrowDown.height * 2));
			}
			else
			{
				Scroller.Trigger.removeEventListener(Event.ENTER_FRAME, scrollItems);
				Scroller.visible = false;
				heroSlots.y = 105;
			}

			while (heroSlots.numChildren > 0)
			{
				heroSlots.removeChildAt(0);
			}			

			for (var i = 0; i < api.HeroDB.length; i++)
			{
				var	hero:Hero = api.HeroDB[i];
				var heroSlot = new warz.frontend.CreateSurvCharSlot();
				
				heroSlot.Btn.addEventListener(MouseEvent.MOUSE_OVER, heroRollOverFn);
				heroSlot.Btn.addEventListener(MouseEvent.MOUSE_OUT, heroRollOutFn);
				heroSlot.Btn.addEventListener(MouseEvent.CLICK, heroRollClickFn);

				heroSlot.x = (i % 3) * 182.45;
				heroSlot.y = int (i / 3) * 202;
				heroSlot.hero = hero;
				
				if (heroSlot.Pic.numChildren > 0)
					heroSlot.Pic.removeChildAt(0);

				heroSlots.addChild(heroSlot);
				
				loadSlotIcon (hero.icon, heroSlot.Pic, 1);
				
				heroSlot.Texts.StatText1.text = hero.name;
				
				var	storeItem:StoreItem = api.getStoreItemByID (hero.itemID);
				var	inventory:InventoryItem = api.getInventoryItemByID(hero.itemID);
				
				if (inventory)
				{
					if (heroSlot.currentLabel != "out")
						heroSlot.gotoAndPlay("out");
				}
				else
				{					
					heroSlot.BtnUnlock.addEventListener(MouseEvent.MOUSE_OVER, unlockRollOverFn);
					heroSlot.BtnUnlock.addEventListener(MouseEvent.MOUSE_OUT, unlockRollOutFn);
					heroSlot.BtnUnlock.addEventListener(MouseEvent.CLICK, unlockRollClickFn);
					if (heroSlot.currentLabel != "locked_out")
						heroSlot.gotoAndPlay("locked_out");
				}				
				
				heroSlot.StoreItem = storeItem;
				heroSlot.Inventory = inventory;				
				
				if (api.getInventoryItemByID (curHero.itemID))
				{			
					BtnCreate.gotoAndStop(1);				
					BtnCreate.State = "off";
				}
				else
				{				
					BtnCreate.gotoAndStop("inactive");				
					BtnCreate.State = "active";
				
				}
			}
		}
		
		public function buyItemSuccessful ()
		{
			updateHeros ();			
		}
		
		public	function SlotPressFn(evt:Event)
		{
			if(evt.currentTarget.parent.State != "active")
			{
				evt.currentTarget.parent.State = "active";
				evt.currentTarget.parent.gotoAndPlay("pressed");
				
				var	p:MovieClip = evt.currentTarget.parent;
				
				while (!(p as ChangeSurvivorScreen))
				{
					p = p.parent as MovieClip;
				}
				
				SoundEvents.eventSoundPlay("menu_click");
				
				(p as ChangeSurvivorScreen).ActionFunction(evt.currentTarget.parent.name);
			}
		}
		
		public	function arrowRollOverFn(evt:Event)
		{
			if(evt.currentTarget && evt.currentTarget.parent && evt.currentTarget.parent.currentFrameLabel!="inactive")
			{
				evt.currentTarget.parent.gotoAndPlay("over");
			}
		}
		
		public	function arrowRollOutFn(evt:Event)
		{
			if(evt.currentTarget && evt.currentTarget.parent && evt.currentTarget.parent.currentFrameLabel!="inactive")
			{
				evt.currentTarget.parent.gotoAndPlay("out");
			}
		}
		
		public	function arrowPressFn(evt:Event)
		{
			if(evt.currentTarget.Arrow.currentFrameLabel!="inactive")
			{
				var	p:MovieClip = (evt.currentTarget as MovieClip);
				
				while (!(p as ChangeSurvivorScreen))
				{
					p = p.parent as MovieClip;
				}
				
				SoundEvents.eventSoundPlay("menu_click");
				
				(p as ChangeSurvivorScreen).ActionFunction(evt.currentTarget.name);
			}
		}
		
		public	function mouseUp(e:Event) :void
		{ 
			ScrollerIsDragging = false;
			Scroller.Trigger.stopDrag(); 
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
		
		public	function scrollerMouseWheel (e:MouseEvent):void
		{
			if (e.stageX > CharacterMask.x && 
				e.stageX < CharacterMask.x + CharacterMask.width + 45 &&
				e.stageY > CharacterMask.y && 
				e.stageY < CharacterMask.y + CharacterMask.height)
			{
				var dist = (heroSlots.height - CharacterMask.height) / 236;
				var h = Scroller.Field.height - Scroller.Trigger.height + 26;
				dist = h / dist;
				
				if (e.delta > 0)
					handleScrollerUpDown(-dist);
				else
					handleScrollerUpDown(dist);
			}
		}

		public function Activate()
		{
			api = warz.frontend.Frontend.api;

			FrontEndEvents.eventRequestPlayerRender (2);

			var	survivor:Survivor = api.Survivors[api.SelectedChar];
			curHero = null;
			for(var j=0; j<api.HeroDB.length; ++j)
			{ 
				if(api.HeroDB[j].itemID == survivor.heroID)
				{
					curHero = api.HeroDB[j];
					break;
				}
			}
			updateHeros();
			body = survivor.bodyID;
			hair = survivor.hairID;
			head = survivor.headID;
			legs = survivor.legsID;
			UpdateArrowStatus();			

			if (curHero)
			{
				FrontEndEvents.eventCreateChangeCharacter (curHero.itemID, hair, head, body, legs);
			}
			
			this.visible = true;
			stage.addEventListener(MouseEvent.MOUSE_WHEEL, scrollerMouseWheel);
			stage.addEventListener(MouseEvent.MOUSE_UP, mouseUp);
		}
		
		public function Deactivate()
		{
			FrontEndEvents.eventRequestPlayerRender(FrontEndEvents.CharRenderDefaultValue);
            stage.removeEventListener(MouseEvent.MOUSE_WHEEL, scrollerMouseWheel);
            stage.removeEventListener(MouseEvent.MOUSE_UP, mouseUp);
			this.visible = false;
		}
		
		public function EventChangeOnSuccess()
		{
			api.Survivors[api.SelectedChar].bodyID = body;
			api.Survivors[api.SelectedChar].hairID = hair;
			api.Survivors[api.SelectedChar].headID = head;
			api.Survivors[api.SelectedChar].legsID = legs;
			api.Survivors[api.SelectedChar].heroID = curHero.itemID;

			api.Main.showScreen("Survivors");
		}
		
		public function ActionFunction (button:String)
		{
			if (button == "BtnBack")
			{
				BtnBack.State = "off";
				BtnBack.gotoAndPlay("out");

				api.Main.showScreen("Survivors");
			}
			else if (button == "BtnCreate")
			{
				BtnCreate.State = "off";
				BtnCreate.gotoAndPlay("out");

				FrontEndEvents.eventChangeOutfit(api.SelectedChar, hair, head, body, legs, curHero.itemID);
			}
			else if (button == "ArrowLeftZoneBtn4")
			{
				Character.ArrowLeftZoneBtn4.Arrow.gotoAndPlay("pressed");
				hair = Math.max(0, --hair);	
				FrontEndEvents.eventCreateChangeCharacter (curHero.itemID, hair, head, body, legs);
				UpdateArrowStatus();			
			}
			else if (button == "ArrowLeftZoneBtn1")
			{
				Character.ArrowLeftZoneBtn1.Arrow.gotoAndPlay("pressed");
				head = Math.max(0, --head);	
				FrontEndEvents.eventCreateChangeCharacter (curHero.itemID, hair, head, body, legs);
				UpdateArrowStatus();			
			}
			else if (button == "ArrowLeftZoneBtn2")
			{
				Character.ArrowLeftZoneBtn2.Arrow.gotoAndPlay("pressed");
				body = Math.max(0, --body);	
				FrontEndEvents.eventCreateChangeCharacter (curHero.itemID, hair, head, body, legs);
				UpdateArrowStatus();
			}
			else if (button == "ArrowLeftZoneBtn3")
			{
				Character.ArrowLeftZoneBtn3.Arrow.gotoAndPlay("pressed");
				legs = Math.max(0, --legs);	
				FrontEndEvents.eventCreateChangeCharacter (curHero.itemID, hair, head, body, legs);
				UpdateArrowStatus();
			}
			else if (button == "ArrowRightZoneBtn4")
			{
				Character.ArrowRightZoneBtn4.Arrow.gotoAndPlay("pressed");
				hair = Math.min(++hair, curHero.numHairs - 1);
				FrontEndEvents.eventCreateChangeCharacter (curHero.itemID, hair, head, body, legs);
				UpdateArrowStatus();				
			}
			else if (button == "ArrowRightZoneBtn1")
			{
				Character.ArrowRightZoneBtn1.Arrow.gotoAndPlay("pressed");
				head = Math.min(++head, curHero.numHeads - 1);
				FrontEndEvents.eventCreateChangeCharacter (curHero.itemID, hair, head, body, legs);
				UpdateArrowStatus();				
			}
			else if (button == "ArrowRightZoneBtn2")
			{
				Character.ArrowRightZoneBtn2.Arrow.gotoAndPlay("pressed");
				body = Math.min(++body, curHero.numBodys - 1);
				FrontEndEvents.eventCreateChangeCharacter (curHero.itemID, hair, head, body, legs);
				UpdateArrowStatus();
			}
			else if (button == "ArrowRightZoneBtn3")
			{
				Character.ArrowRightZoneBtn3.Arrow.gotoAndPlay("pressed");
				legs = Math.min(++legs, curHero.numLegs - 1);
				FrontEndEvents.eventCreateChangeCharacter (curHero.itemID, hair, head, body, legs);
				UpdateArrowStatus();
			}
		}
		
		public function UpdateArrowStatus()
		{
			if(hair == 0)
			{
				if(Character.ArrowLeftZoneBtn4.Arrow.currentFrameLabel != "inactive")
					Character.ArrowLeftZoneBtn4.Arrow.gotoAndPlay("inactive_in");
				if(curHero.numHairs > 1 && Character.ArrowRightZoneBtn4.Arrow.currentFrameLabel == "inactive")
					Character.ArrowRightZoneBtn4.Arrow.gotoAndPlay("inactive_out");
			}
			if(hair == curHero.numHairs-1)
			{
				if(curHero.numHairs > 1 && Character.ArrowLeftZoneBtn4.Arrow.currentFrameLabel == "inactive")
					Character.ArrowLeftZoneBtn4.Arrow.gotoAndPlay("inactive_out");
				if(Character.ArrowRightZoneBtn4.Arrow.currentFrameLabel != "inactive")
					Character.ArrowRightZoneBtn4.Arrow.gotoAndPlay("inactive_in");
			}
			if(hair > 0 && hair < curHero.numHairs-1)
			{
				if(Character.ArrowLeftZoneBtn4.Arrow.currentFrameLabel == "inactive")
					Character.ArrowLeftZoneBtn4.Arrow.gotoAndPlay("inactive_out");
				if(Character.ArrowRightZoneBtn4.Arrow.currentFrameLabel == "inactive")
					Character.ArrowRightZoneBtn4.Arrow.gotoAndPlay("inactive_out");
			}
			
			if(head == 0)
			{
				if(Character.ArrowLeftZoneBtn1.Arrow.currentFrameLabel != "inactive")
					Character.ArrowLeftZoneBtn1.Arrow.gotoAndPlay("inactive_in");
				if(curHero.numHeads > 1 && Character.ArrowRightZoneBtn1.Arrow.currentFrameLabel == "inactive")
					Character.ArrowRightZoneBtn1.Arrow.gotoAndPlay("inactive_out");
			}
			if(head == curHero.numHeads-1)
			{
				if(curHero.numHeads > 1 && Character.ArrowLeftZoneBtn1.Arrow.currentFrameLabel == "inactive")
					Character.ArrowLeftZoneBtn1.Arrow.gotoAndPlay("inactive_out");
				if(Character.ArrowRightZoneBtn1.Arrow.currentFrameLabel != "inactive")
					Character.ArrowRightZoneBtn1.Arrow.gotoAndPlay("inactive_in");
			}
			if(head > 0 && head < curHero.numHeads-1)
			{
				if(Character.ArrowLeftZoneBtn1.Arrow.currentFrameLabel == "inactive")
					Character.ArrowLeftZoneBtn1.Arrow.gotoAndPlay("inactive_out");
				if(Character.ArrowRightZoneBtn1.Arrow.currentFrameLabel == "inactive")
					Character.ArrowRightZoneBtn1.Arrow.gotoAndPlay("inactive_out");
			}
			if(body == 0)
			{
				if(Character.ArrowLeftZoneBtn2.Arrow.currentFrameLabel != "inactive")
					Character.ArrowLeftZoneBtn2.Arrow.gotoAndPlay("inactive_in");
				if(curHero.numBodys>1 && Character.ArrowRightZoneBtn2.Arrow.currentFrameLabel == "inactive")
					Character.ArrowRightZoneBtn2.Arrow.gotoAndPlay("inactive_out");
			}
			if(body == curHero.numBodys-1)
			{
				if(curHero.numBodys>1 && Character.ArrowLeftZoneBtn2.Arrow.currentFrameLabel == "inactive")
					Character.ArrowLeftZoneBtn2.Arrow.gotoAndPlay("inactive_out");
				if(Character.ArrowRightZoneBtn2.Arrow.currentFrameLabel != "inactive")
					Character.ArrowRightZoneBtn2.Arrow.gotoAndPlay("inactive_in");
			}	
			if(body > 0 && body < curHero.numBodys-1)
			{
				if(Character.ArrowLeftZoneBtn2.Arrow.currentFrameLabel == "inactive")
					Character.ArrowLeftZoneBtn2.Arrow.gotoAndPlay("inactive_out");
				if(Character.ArrowRightZoneBtn2.Arrow.currentFrameLabel == "inactive")
					Character.ArrowRightZoneBtn2.Arrow.gotoAndPlay("inactive_out");
			}
			if(legs == 0)
			{
				if(Character.ArrowLeftZoneBtn3.Arrow.currentFrameLabel != "inactive")
					Character.ArrowLeftZoneBtn3.Arrow.gotoAndPlay("inactive_in");
				if(curHero.numLegs>1 && Character.ArrowRightZoneBtn3.Arrow.currentFrameLabel == "inactive")
					Character.ArrowRightZoneBtn3.Arrow.gotoAndPlay("inactive_out");
			}
			if(legs == curHero.numLegs-1)
			{
				if(curHero.numLegs>1 && Character.ArrowLeftZoneBtn3.Arrow.currentFrameLabel == "inactive")
					Character.ArrowLeftZoneBtn3.Arrow.gotoAndPlay("inactive_out");
				if(Character.ArrowRightZoneBtn3.Arrow.currentFrameLabel != "inactive")
					Character.ArrowRightZoneBtn3.Arrow.gotoAndPlay("inactive_in");
			}	
			if(legs > 0 && legs < curHero.numLegs-1)
			{
				if(Character.ArrowLeftZoneBtn3.Arrow.currentFrameLabel == "inactive")
					Character.ArrowLeftZoneBtn3.Arrow.gotoAndPlay("inactive_out");
				if(Character.ArrowRightZoneBtn3.Arrow.currentFrameLabel == "inactive")
					Character.ArrowRightZoneBtn3.Arrow.gotoAndPlay("inactive_out");
			}
		}
	}
}
