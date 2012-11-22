/* ChanServ core functions
 *
 * (C) 2003-2012 Anope Team
 * Contact us at team@anope.org
 *
 * Please read COPYING and README for further details.
 *
 * Based on the original code of Epona by Lara.
 * Based on the original code of Services by Andy Church.
 */

/*************************************************************************/

#include "module.h"

class CommandCSSetPeace : public Command
{
 public:
	CommandCSSetPeace(Module *creator, const Anope::string &cname = "chanserv/set/peace") : Command(creator, cname, 2, 2)
	{
		this->SetDesc(_("Regulate the use of critical commands"));
		this->SetSyntax(_("\037channel\037 {ON | OFF}"));
	}

	void Execute(CommandSource &source, const std::vector<Anope::string> &params) anope_override
	{
		ChannelInfo *ci = ChannelInfo::Find(params[0]);
		if (ci == NULL)
		{
			source.Reply(CHAN_X_NOT_REGISTERED, params[0].c_str());
			return;
		}
		EventReturn MOD_RESULT;
		FOREACH_RESULT(I_OnSetChannelOption, OnSetChannelOption(source, this, ci, params[1]));
		if (MOD_RESULT == EVENT_STOP)
			return;

		if (MOD_RESULT != EVENT_ALLOW && source.permission.empty() && !source.AccessFor(ci).HasPriv("SET"))
		{
			source.Reply(ACCESS_DENIED);
			return;
		}

		if (params[1].equals_ci("ON"))
		{
			ci->SetFlag(CI_PEACE);
			source.Reply(_("Peace option for %s is now \002on\002."), ci->name.c_str());
		}
		else if (params[1].equals_ci("OFF"))
		{
			ci->UnsetFlag(CI_PEACE);
			source.Reply(_("Peace option for %s is now \002off\002."), ci->name.c_str());
		}
		else
			this->OnSyntaxError(source, "PEACE");

		return;
	}

	bool OnHelp(CommandSource &source, const Anope::string &) anope_override
	{
		this->SendSyntax(source);
		source.Reply(" ");
		source.Reply(_("Enables or disables the \002peace\002 option for a channel.\n"
				"When \002peace\002 is set, a user won't be able to kick,\n"
				"ban or remove a channel status of a user that has\n"
				"a level superior or equal to his via %s commands."), source.service->nick.c_str());
		return true;
	}
};

class CSSetPeace : public Module
{
	CommandCSSetPeace commandcssetpeace;

 public:
	CSSetPeace(const Anope::string &modname, const Anope::string &creator) : Module(modname, creator, CORE),
		commandcssetpeace(this)
	{
		this->SetAuthor("Anope");

	}
};

MODULE_INIT(CSSetPeace)
