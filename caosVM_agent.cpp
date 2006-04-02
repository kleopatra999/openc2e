/*
 *  caosVM_agent.cpp
 *  openc2e
 *
 *  Created by Alyssa Milburn on Sun May 30 2004.
 *  Copyright (c) 2004 Alyssa Milburn. All rights reserved.
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 */

#include "caosVM.h"
#include "openc2e.h"
#include "Vehicle.h"
#include "PointerAgent.h"
#include "SkeletalCreature.h"
#include "World.h"
#include "creaturesImage.h"
#include <iostream>

using std::cerr;

SpritePart *caosVM::getCurrentSpritePart() {
	caos_assert(targ);
	CompoundPart *p = targ->part(part);
	caos_assert(p);
	SpritePart *s = dynamic_cast<SpritePart *>(p);
	caos_assert(s);
	return s;
}

/**
 TOUC (integer) first (agent) second (agent)
 %status maybe

 Determines whether the two given agents are touching.  Returns 0 (if not) or 1 (if so).
*/
void caosVM::v_TOUC() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_VALIDAGENT(second)
	VM_PARAM_VALIDAGENT(first)

	result.setInt(0);

	// I did this at 4:30am and have no idea if it works - fuzzie
	if (first->x < second->x) {
		if ((first->x + first->getWidth()) < second->x)
			return;
	} else {
		if ((second->x + second->getWidth()) < first->x)
			return;
	}

	if (first->y < second->y) {
		if ((first->y + first->getHeight()) < second->y)
			return;
	} else {
		if ((second->y + second->getHeight()) < first->y)
			return;
	}

	result.setInt(1);
}

/**
 RTAR (command) family (integer) genus (integer) species (integer)
 %status maybe

 Sets TARG to a random agent with the given family/genus/species.
 */ 
void caosVM::c_RTAR() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	setTarg(0);

	/* XXX: maybe use a map of classifier -> agents? */
	std::vector<Agent *> temp;
	for (std::list<Agent *>::iterator i
		= world.agents.begin(); i != world.agents.end(); i++) {
		
		Agent *a = (*i);
		
		if (species && species != a->species) continue;
		if (genus && genus != a->genus) continue;
		if (family && family != a->family) continue;

		temp.push_back(a);
	}

	if (temp.size() == 0) return;
	int i = rand() % temp.size();
	setTarg(temp[i]);
}

/**
 TTAR (command) family (integer) genus (integer) species (integer)
 %status stub

 Locates a random agent that is touching OWNR (see ETCH) and that 
 matches the given classifier, and sets it to TARG.
*/
void caosVM::c_TTAR() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	
	caos_assert(targ);
	
	setTarg(0); // TODO
}

/**
 STAR (command) family (integer) genus (integer) species (integer)
 %status stub

 Locates a random agent that is visible to OWNR (see ESEE) and that
 matches the given classifier, then sets it to TARG.
*/
void caosVM::c_STAR() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	
	caos_assert(targ);
	
	setTarg(0); // TODO
}

/**
 NEW: SIMP (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe

 Creates a new simple agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, at the screen depth given by plane.
 TARG is set to the newly-created agent.
*/
void caosVM::c_NEW_SIMP() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	SimpleAgent *a = new SimpleAgent(family, genus, species, plane, sprite_file, first_image, image_count);
	setTarg(a);
}

/**
 NEW: COMP (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe

 Creates a new composite agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, with the first part at the screen depth given by plane.
*/
void caosVM::c_NEW_COMP() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	CompoundAgent *a = new CompoundAgent(family, genus, species, plane, sprite_file, first_image, image_count);
	setTarg(a);
}

/**
 NEW: VHCL (command) family (integer) genus (integer) species (integer) sprite_file (string) image_count (integer) first_image (integer) plane (integer)
 %status maybe

 Creates a new vehicle agent with given family/genus/species, given spritefile with image_count sprites
 available starting at first_image in the spritefile, with the first part at the screen depth given by plane.
*/
void caosVM::c_NEW_VHCL() {
	VM_VERIFY_SIZE(7)
	VM_PARAM_INTEGER(plane)
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_INTEGER(image_count)
	VM_PARAM_STRING(sprite_file)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	Vehicle *a = new Vehicle(family, genus, species, plane, sprite_file, first_image, image_count);
	setTarg(a);
}

/**
 TARG (agent)
 %status maybe

 Returns TARG, the currently-targeted agent.
*/
void caosVM::v_TARG() {
	VM_VERIFY_SIZE(0)
	result.setAgent(targ);
}

/**
 OWNR (agent)
 %status maybe
 
 Returns OWNR, the agent that is running the script.
*/
void caosVM::v_OWNR() {
	VM_VERIFY_SIZE(0)
	result.setAgent(owner);
}

/**
 NULL (agent)
 %status maybe

 Returns a null (zero) agent.
*/
void caosVM::v_NULL() {
	static const AgentRef nullref;
	VM_VERIFY_SIZE(0)
	result.setAgent(nullref);
}

/**
 POSE (command) pose (integer)
 %status maybe

 Sets the displayed sprite of TARG to the frame in the sprite file with the given integer.
*/
void caosVM::c_POSE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(pose)

	SkeletalCreature *s = dynamic_cast<SkeletalCreature *>(targ.get());
	if (s) return; // TODO

	SpritePart *p = getCurrentSpritePart();
	caos_assert(p->getFirstImg() + p->getBase() + pose < p->getSprite()->numframes());
	p->setPose(pose);
}

/**
 ATTR (command) attr (integer)
 %status maybe

 Sets attributes of the TARG agent.
.*/
void caosVM::c_ATTR() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(attr)
	caos_assert(targ);
	targ->setAttributes(attr);
}

/**
 ATTR (integer)
 %status maybe

 Returns attributes of the TARG agent.
*/
void caosVM::v_ATTR() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setInt(targ->getAttributes());
}

/**
 TICK (command) tickrate (integer)
 %status maybe

 Initiates the agent timer-- the Timer script will then be run once every tickrate ticks.
 Setting tickrate to zero will stop the timer.
*/
void caosVM::c_TICK() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(tickrate)
	caos_assert(targ);
	targ->setTimerRate(tickrate);
}

/**
 BHVR (command) bhvr (integer)
 %status maybe

 Sets the behaviour of the TARG agent.
*/
void caosVM::c_BHVR() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(bhvr)
	
	caos_assert(targ);

	// reset bhvr
	targ->cr_can_push = targ->cr_can_pull = targ->cr_can_stop =
		targ->cr_can_hit = targ->cr_can_eat = targ->cr_can_pickup = false;
	
	if (bhvr & 0x1) // creature can push
		targ->cr_can_push = true;
	if (bhvr & 0x2) // creature can pull
		targ->cr_can_pull = true;
	if (bhvr & 0x4) // creature can stop
		targ->cr_can_stop = true;
	if (bhvr & 0x8) // creature can hit
		targ->cr_can_hit = true;
	if (bhvr & 0x10) // creature can eat
		targ->cr_can_eat = true;
	if (bhvr & 0x20) // creature can pick up
		targ->cr_can_pickup = true;
}

/**
 TARG (command) agent (agent)
 %status maybe

 Sets TARG (the target agent) to the given agent.
*/
void caosVM::c_TARG() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_AGENT(a)
	setTarg(a);
}

/**
 FROM (agent)
 %status maybe

 Returns the agent that sent the message being processed, or NULL if no agent was involved.
*/
void caosVM::v_FROM() {
	result.setAgent(from);
}

/**
 POSE (integer)
 %status maybe

 Returns the number of the frame in the agent's sprite file that is currently being 
 displayed.
*/
void caosVM::v_POSE() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);

	SpritePart *p = getCurrentSpritePart();
	result.setInt(p->getPose());
}

/**
 KILL (command) agent (agent)
 %status maybe

 Destroys the agent in question. However, you cannot destroy PNTR.
 Remember, use DEAD first for Creatures!
*/
void caosVM::c_KILL() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_VALIDAGENT(a)

	if (a != AgentRef(world.hand()))
		a->kill();
}

/**
 ANIM (command) poselist (bytestring)
 %status maybe

 Sets the animation string for TARG, in the format '1 2 3 4'.
 If it ends with '255', loops back to beginning; if it ends with '255 X', loops back to frame X.

 <i>todo: compound agent stuff</i>
*/
void caosVM::c_ANIM() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_BYTESTR(bs)

	caos_assert(targ);

	SpritePart *p = getCurrentSpritePart();
	p->animation = bs;
	
	if (!bs.empty()) p->setFrameNo(0); // TODO: correct?
}

/**
 ANMS (command) poselist (string)
 %status maybe

 Exactly like ANIM, only using a string and not a bytestring for poselist source.
*/
void caosVM::c_ANMS() {
	VM_PARAM_STRING(poselist)

	// TODO: technically, we should parse this properly, also do error checking
	std::vector<unsigned int> animation;

	std::string t;
	for (unsigned int i = 0; i < poselist.size(); i++) {
		if (poselist[i] == ' ')
			if (!t.empty())
				animation.push_back(atoi(t.c_str()));
		else
			t = t + poselist[i];
	}

	SpritePart *p = getCurrentSpritePart();
	p->animation = animation;
	
	if (!animation.empty()) p->setFrameNo(0); // TODO: correct?
}

/**
 ABBA (integer)
 %status maybe

 Returns the first_image (ie, absolute base) value for the current agent/part.
*/
void caosVM::v_ABBA() {
	VM_VERIFY_SIZE(0)
	
	SpritePart *p = getCurrentSpritePart();
	result.setInt(p->getFirstImg());
}

/**
 BASE (command) index (integer)
 %status maybe

 Sets the frame in the TARG agent's spritefile that will be used as its base image.
 This is relative to the first image set with one of the NEW: commands.
*/
void caosVM::c_BASE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(index)

	caos_assert(targ);

	SpritePart *p = getCurrentSpritePart();
	caos_assert(p->getFirstImg() + index + p->getPose() < p->getSprite()->numframes());
	p->setBase(index);
}

/**
 BASE (integer)
 %status maybe

 Returns the frame in the TARG agent's spritefile being used as the BASE image. 
*/
void caosVM::v_BASE() {
	VM_VERIFY_SIZE(0)
			
	SpritePart *p = getCurrentSpritePart();
	result.setInt(p->getBase());
}

/**
 BHVR (integer)
 %status maybe

 Returns the behaviour of the TARG agent.
*/
void caosVM::v_BHVR() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	
	unsigned char bvr = 0;

	if (targ->cr_can_push) bvr += 0x1;
	if (targ->cr_can_pull) bvr += 0x2;
	if (targ->cr_can_stop) bvr += 0x4;
	if (targ->cr_can_hit) bvr += 0x8;
	if (targ->cr_can_eat) bvr += 0x10;
	if (targ->cr_can_pickup) bvr += 0x20;

	result.setInt(bvr);
}

/**
 CARR (agent)
 %status maybe

 Returns the agent that is carrying the TARG agent.  If TARG is not being carried, returns 
 NULL. 
*/
void caosVM::v_CARR() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	
	result.setAgent(targ->carriedby);
}

/**
 FMLY (integer)
 %status maybe

 Returns the family of the TARG agent.
*/
void caosVM::v_FMLY() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setInt(targ->family);
}

/**
 GNUS (integer)
 %status maybe

 Returns the genus of the TARG agent.
*/
void caosVM::v_GNUS() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setInt(targ->genus);
}

/**
 SPCS (integer)
 %status maybe

 Returns the species of the TARG agent.
*/
void caosVM::v_SPCS() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setInt(targ->species);
}

/**
 PLNE (integer)
 %status maybe

 Returns the plane (z-order) of the TARG agent.
*/
void caosVM::v_PLNE() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setInt(targ->getZOrder());
}

/**
 PNTR (agent)
 %status maybe

 Returns the pointer agent (the Hand).
*/
void caosVM::v_PNTR() {
	VM_VERIFY_SIZE(0)
	result.setAgent(world.hand());
}

unsigned int calculateScriptId(unsigned int message_id) {
	// aka, why oh why is this needed? Silly CL.

	switch (message_id) {
		case 2: /* deactivate */
			return 0;
		case 0: /* activate 1 */
		case 1: /* activate 2 */
			return message_id + 1;
	}

	return message_id;
}

/**
 MESG WRIT (command) agent (agent) message_id (integer)
 %status maybe

 Sends a message of type message_id to the given agent.  FROM will be set to OWNR unless 
 there is no agent involved in sending the message.
*/
void caosVM::c_MESG_WRIT() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(message_id)
	VM_PARAM_VALIDAGENT(agent);

	agent->queueScript(calculateScriptId(message_id), owner.get());
}

/**
 MESG WRT+ (command) agent (agent) message_id (integer) param_1 (anything) param_2 (anything) delay (integer)
 %status maybe

 Sends a message of type message_id to the given agent, much like MESG WRIT, but with the 
 addition of parameters.  The message will be sent after waiting the number of ticks set 
 in delay (except doesn't, right now.  Delay must be set to zero for now.)
*/
void caosVM::c_MESG_WRT() {
	VM_VERIFY_SIZE(5)
	VM_PARAM_INTEGER(delay)
	VM_PARAM_VALUE(param_2)
	VM_PARAM_VALUE(param_1)
	VM_PARAM_INTEGER(message_id)
	VM_PARAM_VALIDAGENT(agent)

	// I'm not sure how to handle the 'delay'; is it a background delay, or do we actually block for delay ticks?
	// TODO: fuzzie can't work out how on earth delays work in c2e, someone fixit

	agent->queueScript(calculateScriptId(message_id), owner.get(), param_1, param_2);
}

/**
 TOTL (integer) family (integer) genus (integer) species (integer)
 %status maybe

 Returns the total number of in-game agents matching the given family/genus/species.
*/
void caosVM::v_TOTL() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(species) caos_assert(species >= 0); caos_assert(species <= 65535);
	VM_PARAM_INTEGER(genus) caos_assert(genus >= 0); caos_assert(genus <= 255);
	VM_PARAM_INTEGER(family) caos_assert(family >= 0); caos_assert(family <= 255);

	unsigned int x = 0;
	for (std::list<Agent *>::iterator i = world.agents.begin(); i != world.agents.end(); i++) {
		if ((*i)->family == family)
			if ((*i)->genus == genus)
				if ((*i)->species == species)
					x++;
	}
	result.setInt(x);
}

/**
 SHOW (command) visibility (integer)
 %status maybe
 
 Sets visibility of the TARG agent to cameras. 0 = invisible, 1 = visible.
*/
void caosVM::c_SHOW() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(visibility)
	caos_assert((visibility == 0) || (visibility == 1));
	caos_assert(targ);
	targ->visible = visibility;
}

/**
 POSX (float)
 %status maybe

 Returns the X position of the TARG agent in the world.
*/
void caosVM::v_POSX() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setFloat(targ->x + (targ->getWidth() / 2.0));
}

/**
 POSY (float)
 %status maybe

 Returns the Y position of the TARG agent in the world.
*/
void caosVM::v_POSY() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setFloat(targ->y + (targ->getHeight() / 2.0));
}

/**
 FRAT (command) framerate (integer)
 %status maybe

 Sets the delay between frame changes of the TARG agent or current PART.
 Must be from 1 to 255, 1 being the normal rate, 2 being half as quickly, and so on.
*/
void caosVM::c_FRAT() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(framerate)

	caos_assert(framerate >= 1 && framerate <= 255);
	caos_assert(targ);

	SpritePart *p = getCurrentSpritePart();
	p->setFramerate(framerate);
	p->framedelay = 0;
}

class blockUntilOver : public blockCond {
	protected:
		AgentRef targ;
		int part;
	public:
		blockUntilOver(Agent *t, int p) : targ(t), part(p) {}
		virtual bool operator()() {
			bool blocking;
			int fno, animsize;

			if (!targ) return false;

			caos_assert(targ)
			CompoundPart *s = targ->part(part);
			caos_assert(s);
			SpritePart *p = dynamic_cast<SpritePart *>(s);
			caos_assert(p);
			
			fno = p->getFrameNo();
			animsize = p->animation.size();

			if (fno + 1 == animsize) blocking = false;
			else if (animsize == 0) blocking = false;
			else blocking = true; 
			return blocking;
		}
};
  

/**
 OVER (command)
 %status maybe

 Waits (blocks the TARG agent) until the animation of the TARG agent or PART is over.
*/
void caosVM::c_OVER() {
	caos_assert(targ);
	
	startBlocking(new blockUntilOver(targ, part));
}

/**
 PUHL (command) pose (integer) x (integer) y (integer)
 %status stub

 Sets relative x/y coordinates for TARG's pickup point.
 Pose is -1 for all poses, or a pose relative to the first image specified in NEW: (not BASE).
*/
void caosVM::c_PUHL() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(pose)

	caos_assert(targ);
	// TODO
}

/**
 PUHL (integer) pose (integer) x_or_y (integer)
 %status stub

 Returns the coordinate for TARG's pickup point. x_or_y should be 1 for x, or 2 for y.
*/
void caosVM::v_PUHL() {
	VM_PARAM_INTEGER(x_or_y)
	VM_PARAM_INTEGER(pose)

	caos_assert(targ);
	result.setInt(0); // TODO
}

/**
 POSL (float)
 %status maybe

 Returns the position of the left side of TARG's bounding box.
*/
void caosVM::v_POSL() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setFloat(targ->x);
}

/**
 POST (float)
 %status maybe

 Returns the position of the top side of TARG's bounding box.
*/
void caosVM::v_POST() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setFloat(targ->y);
}

/**
 POSR (float)
 %status maybe

 Returns the position of the right side of TARG's bounding box.
*/
void caosVM::v_POSR() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setFloat(targ->x + targ->getWidth());
}

/**
 POSB (float)
 %status maybe

 Returns the position of the bottom side of TARG's bounding box.
*/
void caosVM::v_POSB() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setFloat(targ->y + targ->getHeight());
}

/**
 WDTH (integer)
 %status maybe

 Returns the TARG agent's width.
*/
void caosVM::v_WDTH() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setInt(targ->getWidth());
}

/**
 PLNE (command) depth (integer)
 %status maybe

 Sets the plane (the z-order) of the TARG agent.  Higher values are closer to the camera.
*/
void caosVM::c_PLNE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_INTEGER(depth)

	caos_assert(targ);
	targ->setZOrder(depth);
}

/**
 TINT (command) red_tint (integer) green_tint (integer) blue_tint (integer) rotation (integer) swap (integer)
 %status maybe

 Sets the tinting of the TARG agent to the given red, blue, and green values.
*/
void caosVM::c_TINT() {
	VM_VERIFY_SIZE(5)
	VM_PARAM_INTEGER(swap)
	VM_PARAM_INTEGER(rotation)
	VM_PARAM_INTEGER(blue_tint)
	VM_PARAM_INTEGER(green_tint)
	VM_PARAM_INTEGER(red_tint)

	assert(red_tint >= 0 && red_tint <= 256);
	assert(green_tint >= 0 && green_tint <= 256);
	assert(blue_tint >= 0 && blue_tint <= 256);
	assert(swap >= 0 && swap <= 256);
	assert(rotation >= 0 && rotation <= 256);

	SpritePart *p = getCurrentSpritePart();
	p->tint(red_tint, green_tint, blue_tint, rotation, swap);
}

/**
 RNGE (command) distance (float)
 %status maybe

 Sets the TARG agent's range (i.e., the distance it can see and hear).
*/
void caosVM::c_RNGE() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_FLOAT(distance)

	caos_assert(targ)
	targ->range = distance;
}

/**
 RNGE (integer)
 %status maybe

 Returns the TARG agent's range.
*/
void caosVM::v_RNGE() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);
	result.setFloat(targ->range);
}

/**
 TRAN (integer) x (integer) y (integer)
 %status maybe

 Tests if the pixel at (x,y) on the TARG agent is transparent.
 Returns 0 or 1.
*/
void caosVM::v_TRAN() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)

	caos_assert(targ);
	CompoundPart *s = targ->part(0); assert(s);
	SpritePart *p = dynamic_cast<SpritePart *>(s);
	caos_assert(p);
	caos_assert(x >= 0 && x <= p->getWidth());
	caos_assert(y >= 0 && y <= p->getHeight());
	if (p->transparentAt(x, y))
		result.setInt(1);
	else
		result.setInt(0);
}
	
/**
 TRAN (command) transparency (integer) part_no (integer)
 %status maybe

 Sets the TARG agent's behaviour with regard to transparency.  If set to 1, invisible 
 parts of the agent can't be clicked.  If 0, anywhere on the agent (including transparent 
 parts) can be clicked.
*/
void caosVM::c_TRAN() {
	VM_VERIFY_SIZE(2)
	VM_PARAM_INTEGER(part_no)
	VM_PARAM_INTEGER(transparency)

	caos_assert(targ);
	// TODO: handle -1?
	CompoundPart *s = targ->part(part_no);
	caos_assert(s);
	SpritePart *p = dynamic_cast<SpritePart *>(s);
	caos_assert(p);
	p->is_transparent = transparency;
}

/**
 HGHT (integer)
 %status maybe

 Returns the TARG agent's height.
*/
void caosVM::v_HGHT() {
	VM_VERIFY_SIZE(0)
	caos_assert(targ);

	result.setInt(targ->getHeight());
}

/**
 HAND (string)
 %status maybe

 Returns the name of the Hand; default is 'hand'.
*/
void caosVM::v_HAND() {
	VM_VERIFY_SIZE(0)

	result.setString(world.hand()->name);
}

/**
 HAND (command) name (string)
 %status maybe

 Sets the name of the Hand.
*/
void caosVM::c_HAND() {
	VM_VERIFY_SIZE(1)
	VM_PARAM_STRING(name)

	world.hand()->name = name;
}

/**
 TICK (integer)
 %status maybe

 Return the agent timer tick rate of the TARG agent.
*/
void caosVM::v_TICK() {
	VM_VERIFY_SIZE(0)

	caos_assert(targ);
	result.setInt(targ->timerrate);
}

/**
 PUPT (command) pose (integer) x (integer) y (integer)
 %status stub

 Sets relative x/y coordinates for the location in the world where the TARG agent picks up 
 objects.  The pose is relative to the first image set in NEW: (not BASE).
*/
void caosVM::c_PUPT() {
	VM_VERIFY_SIZE(3)
	VM_PARAM_INTEGER(y)
	VM_PARAM_INTEGER(x)
	VM_PARAM_INTEGER(pose)

	// TODO
}

/**
 STPT (command)
 %status maybe

 Stop the script running in TARG, if any.
*/
void caosVM::c_STPT() {
	caos_assert(targ)
	targ->stopScript();
}

/**
 DCOR (command) core_on (integer)
 %status done

 Turns the display of the TARG agent's physical core on and off. Gives a general idea of 
 its size and location (including invisible agents).
*/
void caosVM::c_DCOR() {
	VM_PARAM_INTEGER(core_on)

	caos_assert(targ);
	targ->displaycore = core_on;
}

/**
 MIRA (integer)
 %status stub

 Turns mirroring of the TARG agent's current sprite on or off (0 or 1).
*/
void caosVM::v_MIRA() {
	caos_assert(targ);

	result.setInt(0); // TODO
}
 
/**
 MIRA (command) mirror_on (integer)
 %status stub

 Determines whether or not the TARG agent's current sprite is mirrored. Returns 0 or 1.
*/
void caosVM::c_MIRA() {
	VM_PARAM_INTEGER(mirror_on)

	caos_assert(targ);

	// TODO
}

/**
 DISQ (float) other (agent)
 %status maybe

 Calculates the square of the distance between the centers of the TARG agent and the given 
 agent.
*/
void caosVM::v_DISQ() {
	VM_PARAM_VALIDAGENT(other)

	caos_assert(targ);
	
	float x = (targ->x + (targ->getWidth() / 2)) - (other->x + (other->getWidth() / 2));
	float y = (targ->y + (targ->getHeight() / 2)) - (other->y + (other->getHeight() / 2));

	result.setFloat(x*x + y*y);
}

/**
 ALPH (command) alpha_value (integer) enable (integer)
 %status maybe

 Sets the degree of alpha blending on the TARG agent, to a value from 0 (solid) to 256 
 (invisible). The second parameter will turn alpha blending on and off.
*/
void caosVM::c_ALPH() {
	VM_PARAM_INTEGER(enable)
	VM_PARAM_INTEGER(alpha_value)
	
	if (alpha_value < 0) alpha_value = 0;
	else if (alpha_value > 255) alpha_value = 255;

	caos_assert(targ);

	CompoundAgent *c = dynamic_cast<CompoundAgent *>(targ.get());
	if (c && part == -1) {
		for (std::vector<CompoundPart *>::iterator i = c->parts.begin(); i != c->parts.end(); i++) {
			(*i)->has_alpha = enable;
			(*i)->alpha = alpha_value;
		}
	} else {
		CompoundPart *p = targ->part(part);
		caos_assert(p);
		p->has_alpha = enable;
		p->alpha = alpha_value;
	}
}

/**
 HELD (agent)
 %status stub

 Returns the agent currently held by the TARG agent, or a random one if there are more than one.
*/
void caosVM::v_HELD() {
	result.setAgent(0);

	// TODO
}

/**
 GALL (command) spritefile (string) first_image (integer)
 %status maybe

 Changes the sprite file and first image associated with the TARG agent or current PART.
*/
void caosVM::c_GALL() {
	VM_PARAM_INTEGER(first_image)
	VM_PARAM_STRING(spritefile)

	SpritePart *p = getCurrentSpritePart();
	p->changeSprite(spritefile, first_image);
}

/**
 GALL (string)
 %status maybe

 Returns the name of the sprite file associated with the TARG agent or current PART.
*/
void caosVM::v_GALL() {
	SpritePart *p = getCurrentSpritePart();
	result.setString(p->getSprite()->name);
}

/**
 SEEE (integer) first (agent) second (agent)
 %status maybe

 Returns 1 if the first agent can see the other, or 0 otherwise.
*/
void caosVM::v_SEEE() {
	VM_PARAM_VALIDAGENT(second)
	VM_PARAM_VALIDAGENT(first)

	// TODO: handle walls, creature invisibility
	float x = (first->x + (first->getWidth() / 2)) - (second->x + (second->getWidth() / 2));
	float y = (first->y + (first->getHeight() / 2)) - (second->y + (second->getHeight() / 2));
	float z = sqrt(x*x + y*y);

	if (z > first->range)
		result.setInt(0);
	else
		result.setInt(1);
}

/**
 TINT (integer) attribute (integer)
 %status stub

 Returns the tint value for TARG agent. Pass 1 for red, 2 for blue, 3 for green, 4 for rotation or 5 for swap.
*/
void caosVM::v_TINT() {
	VM_PARAM_INTEGER(attribute)

	caos_assert(targ);
	result.setInt(0); // TODO
}

/**
 TINO (command) red (integer) green (integer) blue (integer) rotation (integer) swap (integer)
 %status stub

 Works like the TINT command, but only applies the tint to the current frame, and discards the rest.
*/
void caosVM::c_TINO() {
	VM_PARAM_INTEGER(swap)
	VM_PARAM_INTEGER(rotation)
	VM_PARAM_INTEGER(blue)
	VM_PARAM_INTEGER(green)
	VM_PARAM_INTEGER(red)

	// TODO
}

/**
 DROP (command)
 %status stub

 Causes the TARG agent to drop what it is carrying in a safe location.
*/
void caosVM::c_DROP() {
	caos_assert(targ);

	// TODO
}

AgentRef findNextAgent(AgentRef previous, unsigned char family, unsigned char genus, unsigned short species, bool forward) {
	if (world.agents.size() == 0) return AgentRef(); // shouldn't happen..
	
	AgentRef firstagent;
	bool foundagent = false;

	std::list<Agent *>::iterator i;
	if (forward)
		i = world.agents.begin();
	else {
		// TODO: i doubt this works
		i = world.agents.end();
		i--;
	}

	// Loop through all the agents.
	while (true) {
		if ((*i)->family == family || family == 0)
			if ((*i)->genus == genus || genus == 0)
				if ((*i)->species == species || species == 0) {
					if (!firstagent) firstagent = *i;
					if (foundagent) return *i; // This is the agent we want!
					if (*i == previous) foundagent = true;
				}
		
		// Step through the list. Break if we need to.
		if (!forward && i == world.agents.begin()) break;
		if (forward) i++; else i--;
		if (forward && i == world.agents.end()) break;
	}
	
	// Either we didn't find the previous agent, or we're at the end. Either way, return the first agent found.
	return firstagent;
}

/**
 NCLS (agent) previous (agent) family (integer) genus (integer) species (integer)
 %status maybe
*/
void caosVM::v_NCLS() {
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	VM_PARAM_AGENT(previous)

	result.setAgent(findNextAgent(previous, family, genus, species, true));
}

/**
 PCLS (agent) previous (agent) family (integer) genus (integer) species (integer)
 %status maybe
*/
void caosVM::v_PCLS() {
	VM_PARAM_INTEGER(species)
	VM_PARAM_INTEGER(genus)
	VM_PARAM_INTEGER(family)
	VM_PARAM_AGENT(previous)

	result.setAgent(findNextAgent(previous, family, genus, species, false));
}

/* vim: set noet: */
