/* The server side of the network stuff */

/* The following is a direct excerpt from the netserver.c
 * in the XPilot distribution.  Much of it is incorrect
 * in MAngband's case, but most of it is still correct.
 */



/*
 * This is the server side of the network connnection stuff.
 *
 * We try very hard to not let the game be disturbed by
 * players logging in.  Therefore a new connection
 * passes through several states before it is actively
 * playing.
 * First we make a new connection structure available
 * with a new socket to listen on.  This socket port
 * number is told to the client via the pack mechanism.
 * In this state the client has to send a packet to this
 * newly created socket with its name and playing parameters.
 * If this succeeds the connection advances to its second state.
 * In this second state the essential server configuration
 * like the map and so on is transmitted to the client.
 * If the client has acknowledged all this data then it
 * advances to the third state, which is the
 * ready-but-not-playing-yet state.  In this state the client
 * has some time to do its final initializations, like mapping
 * its user interface windows and so on.
 * When the client is ready to accept frame updates and process
 * keyboard events then it sends the start-play packet.
 * This play packet advances the connection state into the
 * actively-playing state.  A player structure is allocated and
 * initialized and the other human players are told about this new player.
 * The newly started client is told about the already playing players and
 * play has begun.
 * Apart from these four states there are also two intermediate states.
 * These intermediate states are entered when the previous state
 * has filled the reliable data buffer and the client has not
 * acknowledged all the data yet that is in this reliable data buffer.
 * They are so called output drain states.  Not doing anything else
 * then waiting until the buffer is empty.
 * The difference between these two intermediate states is tricky.
 * The second intermediate state is entered after the
 * ready-but-not-playing-yet state and before the actively-playing state.
 * The difference being that in this second intermediate state the client
 * is already considered an active player by the rest of the server
 * but should not get frame updates yet until it has acknowledged its last
 * reliable data.
 *
 * Communication between the server and the clients is only done
 * using UDP datagrams.  The first client/serverized version of XPilot
 * was using TCP only, but this was too unplayable across the Internet,
 * because TCP is a data stream always sending the next byte.
 * If a packet gets lost then the server has to wait for a
 * timeout before a retransmission can occur.  This is too slow
 * for a real-time program like this game, which is more interested
 * in recent events than in sequenced/reliable events.
 * Therefore UDP is now used which gives more network control to the
 * program.
 * Because some data is considered crucial, like the names of
 * new players and so on, there also had to be a mechanism which
 * enabled reliable data transmission.  Here this is done by creating
 * a data stream which is piggybacked on top of the unreliable data
 * packets.  The client acknowledges this reliable data by sending
 * its byte position in the reliable data stream.  So if the client gets
 * a new reliable data packet and it has not had this data before and
 * there is also no data packet missing inbetween, then it advances
 * its byte position and acknowledges this new position to the server.
 * Otherwise it discards the packet and sends its old byte position
 * to the server meaning that it detected a packet loss.
 * The server maintains an acknowledgement timeout timer for each
 * connection so that it can retransmit a reliable data packet
 * if the acknowledgement timer expires.
 */



#define SERVER

#include "angband.h"
#include "netserver.h"

#define MAX_SELECT_FD			1023
/* #define MAX_RELIABLE_DATA_PACKET_SIZE	1024 */
#define MAX_RELIABLE_DATA_PACKET_SIZE	512

#define MAX_MOTD_CHUNK			512
#define MAX_MOTD_SIZE			(30*1024)
#define MAX_MOTD_LOOPS			120


static connection_t	*Conn = NULL;
static int		max_connections = 0;
static setup_t		Setup;
static int		(*playing_receive[256])(int ind),
			(*login_receive[256])(int ind),
			(*drain_receive[256])(int ind);
int			login_in_progress;
static int		num_logins, num_logouts;
static long		Id;
int			NumPlayers;

static int		MetaSocket = -1;

#ifdef NEW_SERVER_CONSOLE
static int		ConsoleSocket = -1;
#endif

static void Contact(int fd, int arg);
static void Console(int fd, int arg);
static int Enter_player(char *real, char *name, char *addr, char *host,
				unsigned version, int port, int *login_port);

char *showtime(void)
{
	time_t		now;
	struct tm	*tmp;
	static char	month_names[13][4] = {
				"Jan", "Feb", "Mar", "Apr", "May", "Jun",
				"Jul", "Aug", "Sep", "Oct", "Nov", "Dec",
				"Bug"
			};
	static char	buf[80];

	time(&now);
	tmp = localtime(&now);
	sprintf(buf, "%02d %s %02d:%02d:%02d",
		tmp->tm_mday, month_names[tmp->tm_mon],
		tmp->tm_hour, tmp->tm_min, tmp->tm_sec);
	return buf;
}


/*
 * Initialize the function dispatch tables for the various client
 * connection states.  Some states use the same table.
 */
static void Init_receive(void)
{
	int i;

	for (i = 0; i < 256; i++)
	{
		login_receive[i] = Receive_undefined;
		playing_receive[i] = Receive_undefined;
		drain_receive[i] = Receive_undefined;
	}

	drain_receive[PKT_QUIT]			= Receive_quit;
	drain_receive[PKT_ACK]			= Receive_ack;
	drain_receive[PKT_VERIFY]		= Receive_discard;
	drain_receive[PKT_PLAY]			= Receive_discard;

	login_receive[PKT_PLAY]			= Receive_play;
	login_receive[PKT_QUIT]			= Receive_quit;
	login_receive[PKT_ACK]			= Receive_ack;
	login_receive[PKT_VERIFY]		= Receive_discard;
	
	playing_receive[PKT_ACK]		= Receive_ack;
	playing_receive[PKT_VERIFY]		= Receive_discard;
	playing_receive[PKT_QUIT]		= Receive_quit;
	playing_receive[PKT_PLAY]		= Receive_play;

	playing_receive[PKT_WALK]		= Receive_walk;
	playing_receive[PKT_RUN]		= Receive_run;
	playing_receive[PKT_TUNNEL]		= Receive_tunnel;
	playing_receive[PKT_AIM_WAND]		= Receive_aim_wand;
	playing_receive[PKT_DROP]		= Receive_drop;
	playing_receive[PKT_FIRE]		= Receive_fire;
	playing_receive[PKT_STAND]		= Receive_stand;
	playing_receive[PKT_DESTROY]		= Receive_destroy;
	playing_receive[PKT_LOOK]		= Receive_look;
	playing_receive[PKT_SPELL]		= Receive_spell;

	playing_receive[PKT_OPEN]		= Receive_open;
	playing_receive[PKT_PRAY]		= Receive_pray;
	playing_receive[PKT_QUAFF]		= Receive_quaff;
	playing_receive[PKT_READ]		= Receive_read;
	playing_receive[PKT_SEARCH]		= Receive_search;
	playing_receive[PKT_TAKE_OFF]		= Receive_take_off;
	playing_receive[PKT_USE]		= Receive_use;
	playing_receive[PKT_THROW]		= Receive_throw;
	playing_receive[PKT_WIELD]		= Receive_wield;
	playing_receive[PKT_ZAP]		= Receive_zap;

	playing_receive[PKT_TARGET]		= Receive_target;
	playing_receive[PKT_TARGET_FRIENDLY]	= Receive_target_friendly;
	playing_receive[PKT_INSCRIBE]		= Receive_inscribe;
	playing_receive[PKT_UNINSCRIBE]		= Receive_uninscribe;
	playing_receive[PKT_ACTIVATE]		= Receive_activate;
	playing_receive[PKT_BASH]		= Receive_bash;
	playing_receive[PKT_DISARM]		= Receive_disarm;
	playing_receive[PKT_EAT]		= Receive_eat;
	playing_receive[PKT_FILL]		= Receive_fill;
	playing_receive[PKT_LOCATE]		= Receive_locate;
	playing_receive[PKT_MAP]		= Receive_map;
	playing_receive[PKT_SEARCH_MODE]	= Receive_search_mode;

	playing_receive[PKT_CLOSE]		= Receive_close;
	playing_receive[PKT_GAIN]		= Receive_gain;
	playing_receive[PKT_DIRECTION]		= Receive_direction;
	playing_receive[PKT_GO_UP]		= Receive_go_up;
	playing_receive[PKT_GO_DOWN]		= Receive_go_down;
	playing_receive[PKT_MESSAGE]		= Receive_message;
	playing_receive[PKT_ITEM]		= Receive_item;
	playing_receive[PKT_PURCHASE]		= Receive_purchase;

	playing_receive[PKT_SELL]		= Receive_sell;
	playing_receive[PKT_STORE_LEAVE]	= Receive_store_leave;
	playing_receive[PKT_STORE_CONFIRM]	= Receive_store_confirm;
	playing_receive[PKT_DROP_GOLD]		= Receive_drop_gold;
	playing_receive[PKT_REDRAW]		= Receive_redraw;
	playing_receive[PKT_REST]		= Receive_rest;
	playing_receive[PKT_SPECIAL_LINE]	= Receive_special_line;
	playing_receive[PKT_PARTY]		= Receive_party;
	playing_receive[PKT_GHOST]		= Receive_ghost;

	playing_receive[PKT_STEAL]		= Receive_steal;
	playing_receive[PKT_OPTIONS]		= Receive_options;
	playing_receive[PKT_SUICIDE]		= Receive_suicide;
	playing_receive[PKT_MASTER]		= Receive_master;

	playing_receive[PKT_AUTOPHASE]		= Receive_autophase;
}

static int Init_setup(void)
{
	int n = 0;
	char buf[1024];
	FILE *fp;

	Setup.frames_per_second = FPS;
	Setup.motd_len = 23 * 80;
	Setup.setup_size = sizeof(setup_t);

	path_build(buf, 1024, ANGBAND_DIR_FILE, "news.txt");

	/* Open the news file */
	fp = my_fopen(buf, "r");

	if (fp)
	{
		/* Dump the file into the buffer */
		while (0 == my_fgets(fp, buf, 1024) && n < 23)
		{
			strncpy(&Setup.motd[n * 80], buf, 80);
			n++;
		}

		my_fclose(fp);
	}
	
	return 0;
}
	

/*
 * Talk to the metaserver.
 *
 * This function is called on startup, on death, and when the number of players
 * in the game changes.
 */
bool Report_to_meta(int flag)
{
	static sockbuf_t meta_buf;
	static char local_name[1024];
	static int init = 0;
	int bytes, i;
	char buf[1024], temp[100];
	bool dungeon_master = 0;

	/* Abort if the user doesn't want to report */
	if (strlen(META_ADDRESS) == 0)
		return FALSE;

	/* If this is the first time called, initialize our hostname */
	if (!init)
	{
		/* Never do this again */
		init = 1;

		/* Get our hostname */
#ifdef BIND_NAME
		strncpy( local_name, BIND_NAME, 1024);
#else
		GetLocalHostName(local_name, 1024);
#endif
	}

	strcpy(buf, local_name);

	if (flag & META_START)
	{
		if ((MetaSocket = CreateDgramSocket(0)) == -1)
		{
			quit("Couldn't create meta-server Dgram socket\n");
		}

		if (SetSocketNonBlocking(MetaSocket, 1) == -1)
		{
			quit("Can't make socket non-blocking\n");
		}

		if (Sockbuf_init(&meta_buf, MetaSocket, SERVER_SEND_SIZE,
			SOCKBUF_READ | SOCKBUF_WRITE | SOCKBUF_DGRAM) == -1)
		{
			quit("No memory for sockbuf buffer\n");
		}

		Sockbuf_clear(&meta_buf);

		strcat(buf, " Number of players: 0  ");
	}

	else if (flag & META_DIE)
	{
		strcat(buf, " ");
	}

	else if (flag & META_UPDATE)
	{
		strcat(buf, " Number of players: ");

		/* Hack -- determine if the DungeonMaster is playing, if so, reduce the number
		 * of players reported.  This is to keep the users clueless as to whether or not
		 * a dungeonmaster is in the game. -APD
		 */

		for (i = 1; i <= NumPlayers; i++)
		{
			if (!strcmp(Players[i]->name, DUNGEON_MASTER)) dungeon_master = TRUE;	
		}

		/* tell the metaserver about everyone except the dungeon_master */
		sprintf(temp, "%d ", NumPlayers - dungeon_master);
		strcat(buf, temp);

		/* if someone other than a dungeon master is playing */
		if (NumPlayers - dungeon_master)
		{
			strcat(buf, "Names: ");
			for (i = 1; i <= NumPlayers; i++)
			{
				/* Hack -- DungeonMaster does not appear on metaserver list */
				if (!strcmp(Players[i]->name, DUNGEON_MASTER)) continue;
				strcat(buf, Players[i]->basename);
				strcat(buf, " ");
			}
		}
	}

	/* Append the version number */
	sprintf(temp, "Version: %d.%d.%d ", VERSION_MAJOR, VERSION_MINOR, VERSION_PATCH);

	/* Append the additional version info */
	if (VERSION_EXTRA == 1)
		strcat(temp, "alpha");
	if (VERSION_EXTRA == 2)
		strcat(temp, "beta");
	if (VERSION_EXTRA == 3)
		strcat(temp, "development");

	if (!(flag & META_DIE))
		strcat(buf, temp);

	/* If we haven't setup the meta connection yet, abort */
	if (MetaSocket == -1)
		return FALSE;

	Sockbuf_clear(&meta_buf);

	Packet_printf(&meta_buf, "%S", buf);

	if ((bytes = DgramSend(MetaSocket, META_ADDRESS, 8800, meta_buf.buf, meta_buf.len) == -1))
	{
		s_printf("Couldn't send info to meta-server!\n");
		return FALSE;
	}

	return TRUE;
}

	 
/*
 * Initialize the connection structures.
 */
int Setup_net_server(void)
{
	size_t size;

	Init_receive();

	if (Init_setup() == -1)
		return -1;

	/*
	 * The number of connections is limited by the number of bases
	 * and the max number of possible file descriptors to use in
	 * the select(2) call minus those for stdin, stdout, stderr,
	 * the contact socket, and the socket for the resolver library routines.
	 */

	max_connections = MAX_SELECT_FD - 5;
	size = max_connections * sizeof(*Conn);
	if ((Conn = (connection_t *) malloc(size)) == NULL)
		quit("Cannot allocate memory for connections");

	memset(Conn, 0, size);

	C_MAKE(Players, max_connections, player_type *);

	/* Tell the metaserver that we're starting up */
	Report_to_meta(META_START);

	s_printf("Server is running version %04x\n", MY_VERSION);

	return 0;
}

/* The contact socket */
static int Socket;
static sockbuf_t ibuf;

void setup_contact_socket(void)
{
	if ((Socket = CreateDgramSocket(18346)) == -1)
	{
		quit("Could not create Dgram socket");
	}
	if (SetSocketNonBlocking(Socket, 1) == -1)
	{
		quit("Can't make contact socket non-blocking");
	}
	if (Sockbuf_init(&ibuf, Socket, SERVER_SEND_SIZE,
		SOCKBUF_READ | SOCKBUF_WRITE | SOCKBUF_DGRAM) == -1)
	{
		quit("No memory for contact buffer");
	}

	install_input(Contact, Socket, 0);
	
#ifdef SERVER_CONSOLE
	/* Hack -- Install stdin an the "console" input */
	install_input(Console, 0, 0);
#endif

#ifdef NEW_SERVER_CONSOLE
	if ((ConsoleSocket = CreateDgramSocket(18347)) == -1)
	{
		s_printf("Couldn't create console socket\n");
		return;
	}

	if (!InitNewConsole(ConsoleSocket))
	{
		return;
	}

	/* Install the new console socket */
	install_input(NewConsole, ConsoleSocket, 0);
#endif
}

static int Reply(char *host_addr, int port)
{
	int i, result;

	for (i = 0; i < 3; i++)
	{
		if ((result = DgramSend(ibuf.sock, host_addr, port, ibuf.buf, ibuf.len)) == -1)
		{
			GetSocketError(ibuf.sock);
		}
		else break;
	}

	return result;
}


static int Check_names(char *nick_name, char *real_name, char *host_name, char *addr)
{
	player_type *p_ptr;
	char *ptr;
	int i;

	if (real_name[0] == 0 || host_name[0] == 0 || nick_name[0] < 'A' ||
		nick_name[0] > 'Z')
		return E_INVAL;

	for (ptr = &nick_name[strlen(nick_name)]; ptr-- > nick_name; )
	{
		if (isascii(*ptr) && isspace(*ptr))
			*ptr = '\0';
		else break;
	}

	for (i = 1; i < NumPlayers + 1; i++)
	{
		p_ptr = Players[i];
		if (strcasecmp(p_ptr->name, nick_name) == 0)
		{
			/*plog(format("%s %s", Players[i]->name, nick_name));*/

			/* The following code allows you to "override" an
			 * existing connection by connecting again  -Crimson */

			/* XXX Hack -- since the password is not read until later, to
			 * authorize the "hijacking" of an existing connection,
			 * we check to see if the username and hostname are
			 * identical.  Note that it may be possobile to spoof this,
			 * kicking someone off.  This is a quick hack that should 
			 * be replaced with proper password checking. 
			 */
			if ((!strcasecmp(p_ptr->realname, real_name)) && (!strcasecmp(p_ptr->addr, addr)))	
				Destroy_connection(p_ptr->conn, "resume connection");
			else return E_IN_USE;
		}
		
		/* All restrictions on the number of allowed players from one IP have 
		 * been removed at this time. -APD
		 *
		
		if (!strcasecmp(Players[i]->realname, real_name) &&
		    !strcasecmp(Players[i]->addr, addr) &&
		    strcasecmp(Players[i]->realname, "mangband"))
		{
			return E_TWO_PLAYERS;
		}
		
		*/
	}

	return SUCCESS;
}

static void Console(int fd, int arg)
{
	char buf[1024];
	int i;

	/* See what we got */
        /* this code added by thaler, 6/28/97 */
        fgets(buf, 1024, stdin);
        if (buf[ strlen(buf)-1 ] == '\n')
            buf[ strlen(buf)-1 ] = '\0';

	for (i = 0; i < strlen(buf) && buf[i] != ' '; i++)
	{
		/* Capitalize each letter until we hit a space */
		buf[i] = toupper(buf[i]);
	}

	/* Process our input */
	if (!strncmp(buf, "HELLO", 5))
		s_printf("Hello.  How are you?\n");

	if (!strncmp(buf, "SHUTDOWN", 8))
	{
		shutdown_server();
	}

		
	if (!strncmp(buf, "STATUS", 6))
	{
		s_printf("There %s %d %s.\n", (NumPlayers != 1 ? "are" : "is"), NumPlayers, (NumPlayers != 1 ? "players" : "player"));

		if (NumPlayers > 0)
		{
			s_printf("%s:\n", (NumPlayers > 1 ? "They are" : "He is"));
			for (i = 1; i < NumPlayers + 1; i++)
				s_printf("\t%s\n", Players[i]->name);
		}
	}

	if (!strncmp(buf, "MESSAGE", 7))
	{
		/* Send message to all players */
		for (i = 1; i <= NumPlayers; i++)
			msg_format(i, "[Server Admin] %s", &buf[8]);

		/* Acknowledge */
		s_printf("Message sent.\n");
	}
		
	if (!strncmp(buf, "KELDON", 6))
	{
		/* Whatever I need at the moment */
	}
}
		
static void Contact(int fd, int arg)
{
	int bytes, login_port;
	u16b version = 0;
	unsigned magic;
	unsigned short port;
	char	ch,
		real_name[MAX_CHARS],
		nick_name[MAX_CHARS],
		host_name[MAX_CHARS],
		host_addr[24],
		reply_to, status;

	/*
	 * Someone connected to us, now try and decipher the message
	 */
	Sockbuf_clear(&ibuf);
	if ((bytes = DgramReceiveAny(Socket, ibuf.buf, ibuf.size)) <= 8)
	{
		if (bytes < 0 && errno != EWOULDBLOCK && errno != EAGAIN &&
			errno != EINTR)
		{
			/* Clear the error condition for the contact socket */
			GetSocketError(Socket);
		}
		return;
	}
	ibuf.len = bytes;

	strcpy(host_addr, DgramLastaddr());
	/*if (Check_address(host_addr)) return;*/

	if (Packet_scanf(&ibuf, "%u", &magic) <= 0)
	{
		plog(format("Incompatible packet from %s", host_addr));
		return;
	}

	if (Packet_scanf(&ibuf, "%s%hu%c", real_name, &port, &ch) <= 0)
	{
		plog(format("Incomplete packet from %s", host_addr));
		return;
	}
	reply_to = (ch & 0xFF);

	port = DgramLastport();

	if (Packet_scanf(&ibuf, "%s%s%hu", nick_name, host_name, &version) <= 0)
	{
		plog(format("Incomplete login from %s", host_addr));
		return;
	}
	nick_name[sizeof(nick_name) - 1] = '\0';
	host_name[sizeof(host_name) - 1] = '\0';

#if 0
	s_printf("Received contact from %s:%d.\n", host_name, port);
	s_printf("Address: %s.\n", host_addr);
	s_printf("Info: real_name %s, port %hu, nick %s, host %s, version %hu\n", real_name, port, nick_name, host_name, version);  
#endif


	status = Enter_player(real_name, nick_name, host_addr, host_name,
				version, port, &login_port);

	Sockbuf_clear(&ibuf);

	/* s_printf("Sending login port %d, status %d.\n", login_port, status); */

	Packet_printf(&ibuf, "%c%c%d", reply_to, status, login_port);

	Reply(host_addr, port);
}

static int Enter_player(char *real, char *nick, char *addr, char *host,
				unsigned version, int port, int *login_port)
{
	int status;

	*login_port = 0;

	if (NumPlayers >= MAX_SELECT_FD)
		return E_GAME_FULL;

	if ((status = Check_names(nick, real, host, addr)) != SUCCESS)
	{
		/*s_printf("Check_names failed with result %d.\n", status);*/
		return status;
	}

	if (version < 0x0420)
	{
		return E_VERSION;
	}

	*login_port = Setup_connection(real, nick, addr, host, version);

	if (*login_port == -1)
		return E_SOCKET;

	if (!lookup_player_id(nick))
		return E_NEED_INFO;

	return SUCCESS;
}
		

static void Conn_set_state(connection_t *connp, int state, int drain_state)
{
	static int num_conn_busy;
	static int num_conn_playing;

	if ((connp->state & (CONN_PLAYING | CONN_READY)) != 0)
		num_conn_playing--;
	else if (connp->state == CONN_FREE)
		num_conn_busy++;

	connp->state = state;
	connp->drain_state = drain_state;
	connp->start = turn;

	if (connp->state == CONN_PLAYING)
	{
		num_conn_playing++;
		connp->timeout = IDLE_TIMEOUT;
	}
	else if (connp->state == CONN_READY)
	{
		num_conn_playing++;
		connp->timeout = READY_TIMEOUT;
	}
	else if (connp->state == CONN_LOGIN)
		connp->timeout = LOGIN_TIMEOUT;
	else if (connp->state == CONN_SETUP)
		connp->timeout = SETUP_TIMEOUT;
	else if (connp->state == CONN_LISTENING)
		connp->timeout = LISTEN_TIMEOUT;
	else if (connp->state == CONN_FREE)
	{
		num_conn_busy--;
		connp->timeout = IDLE_TIMEOUT;
	}
	login_in_progress = num_conn_busy - num_conn_playing;
}


/*
 * Delete a player's information and save his game
 */
static void Delete_player(int Ind)
{
	player_type *p_ptr = Players[Ind];
	int i;

	/* Be paranoid */
	if (cave[p_ptr->dun_depth])
	{
		/* There's nobody on this space anymore */
		cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].m_idx = 0;

		/* Forget his lite and viewing area */
		forget_lite(Ind);
		forget_view(Ind);

		/* Show everyone his disappearance */
		everyone_lite_spot(p_ptr->dun_depth, p_ptr->py, p_ptr->px);
	}

	/* Try to save his character */
	save_player(Ind);

	/* If he was actively playing, tell everyone that he's left */
	/* Hack -- don't tell people about DungeonMaster */
	if (p_ptr->alive && !p_ptr->death && strcmp(p_ptr->name, DUNGEON_MASTER))
	{
		for (i = 1; i < NumPlayers + 1; i++)
		{
			if (Players[i]->conn == NOT_CONNECTED)
				continue;

			/* Don't tell him about himself */
			if (i == Ind) continue;

			/* Send a little message */
			msg_format(i, "%s has left the game.", p_ptr->name);
		}
	}


	/* Swap entry number 'Ind' with the last one */
	/* Also, update the "player_index" on the cave grids */
	if (Ind != NumPlayers)
	{
		p_ptr			= Players[NumPlayers];
		if (cave[p_ptr->dun_depth])
			cave[p_ptr->dun_depth][p_ptr->py][p_ptr->px].m_idx = 0 - Ind;
		Players[NumPlayers]	= Players[Ind];
		Players[Ind]		= p_ptr;
		p_ptr			= Players[NumPlayers];
	}

	GetInd[Conn[Players[Ind]->conn].id] = Ind;
	GetInd[Conn[Players[NumPlayers]->conn].id] = NumPlayers;

	/* Recalculate player-player visibility */
	update_players();

	if (p_ptr)
	{
		if (p_ptr->inventory)
			C_KILL(p_ptr->inventory, INVEN_TOTAL, object_type);

		KILL(p_ptr, player_type);
	}

	NumPlayers--;

	/* Tell the metaserver about the loss of a player */	
	Report_to_meta(META_UPDATE);
}


/*
 * Cleanup a connection.  The client may not know yet that
 * it is thrown out of the game so we send it a quit packet.
 * We send it twice because of UDP it could get lost.
 * Since 3.0.6 the client receives a short message
 * explaining why the connection was terminated.
 */
bool Destroy_connection(int ind, char *reason)
{
	connection_t		*connp = &Conn[ind];
	int			id, len, sock;
	char			pkt[MAX_CHARS];

	if (connp->state == CONN_FREE)
	{
		errno = 0;
		plog(format("Cannot destroy empty connection (\"%s\")", reason));
		return FALSE;
	}

	sock = connp->w.sock;
	remove_input(sock);

	strncpy(&pkt[1], reason, sizeof(pkt) - 3);
	pkt[sizeof(pkt) - 2] = '\0';
	pkt[0] = PKT_QUIT;
	len = strlen(pkt) + 2;
	pkt[len - 1] = PKT_END;
	pkt[len] = '\0';
	/*len++;*/
	if (DgramWrite(sock, pkt, len) != len)
	{
		GetSocketError(sock);
		DgramWrite(sock, pkt, len);
	}
	s_printf("%s: Goodbye %s=%s@%s (\"%s\")\n",
		showtime(),
		connp->nick ? connp->nick : "",
		connp->real ? connp->real : "",
		connp->host ? connp->host : "",
		reason);

	Conn_set_state(connp, CONN_FREE, CONN_FREE);

	if (connp->id != -1)
	{
		id = connp->id;
		/*connp->id = -1;*/
		/*Players[GetInd[id]]->conn = NOT_CONNECTED;*/
		Delete_player(GetInd[id]);
	}
	if (connp->real != NULL)
		free(connp->real);
	if (connp->nick != NULL)
		free(connp->nick);
	if (connp->addr != NULL)
		free(connp->addr);
	if (connp->host != NULL)
		free(connp->host);
	Sockbuf_cleanup(&connp->w);
	Sockbuf_cleanup(&connp->r);
	Sockbuf_cleanup(&connp->c);
	Sockbuf_cleanup(&connp->q);
	memset(connp, 0, sizeof(*connp));

	num_logouts++;

	if (DgramWrite(sock, pkt, len) != len)
	{
		GetSocketError(sock);
		DgramWrite(sock, pkt, len);
	}
	DgramClose(sock);

	return TRUE;
}

int Check_connection(char *real, char *nick, char *addr)
{
	int i;
	connection_t *connp;

	for (i = 0; i < max_connections; i++)
	{
		connp = &Conn[i];
		if (connp->state == CONN_LISTENING)
			if (strcasecmp(connp->nick, nick) == 0)
			{
				if (!strcmp(real, connp->real)
					&& !strcmp(addr, connp->addr))
						return connp->my_port;
				return -1;
			}
	}
	return -1;
}


/*
 * A client has requested a playing connection with this server.
 * See if we have room for one more player and if his name is not
 * already in use by some other player.  Because the confirmation
 * may get lost we are willing to send it another time if the
 * client connection is still in the CONN_LISTENING state.
 */
int Setup_connection(char *real, char *nick, char *addr, char *host,
			unsigned version)
{
	int i, free_conn_index = max_connections, my_port, sock;
	connection_t *connp;

	for (i = 0; i < max_connections; i++)
	{
		connp = &Conn[i];
		if (connp->state == CONN_FREE)
		{
			if (free_conn_index == max_connections)
				free_conn_index = i;
			continue;
		}
		if (strcasecmp(connp->nick, nick) == 0)
		{
			if (connp->state == CONN_LISTENING
				&& strcmp(real, connp->real) == 0
				&& version == connp->version)
					return connp->my_port;
			else return -1;
		}
	}

	if (free_conn_index >= max_connections)
	{
		s_printf("Full house for %s(%s)@%s\n", real, nick, host);
		return -1;
	}
	connp = &Conn[free_conn_index];

	if ((sock = CreateDgramSocket(0)) == -1)
	{
		plog(format("Cannot create datagram socket (%d)", sl_errno));
		return -1;
	}
	if (sock >= MAX_SELECT_FD)
	{
		errno = 0;
		plog("Socket filedescriptor too big");
		DgramClose(sock);
		return -1;
	}
	if ((my_port = GetPortNum(sock)) == 0)
	{
		plog("Cannot get port from socket");
		DgramClose(sock);
		return -1;
	}
	if (SetSocketNonBlocking(sock, 1) == -1)
	{
		plog("Cannot make client socket non-blocking");
		DgramClose(sock);
		return -1;
	}
	if (SetSocketReceiveBufferSize(sock, SERVER_RECV_SIZE + 256) == -1)
		plog(format("Cannot set receive buffer size to %d", SERVER_RECV_SIZE + 256));
	if (SetSocketSendBufferSize(sock, SERVER_SEND_SIZE + 256) == -1)
		plog(format("Cannot set send buffer size to %d", SERVER_SEND_SIZE + 256));

	Sockbuf_init(&connp->w, sock, SERVER_SEND_SIZE, SOCKBUF_WRITE | SOCKBUF_DGRAM);
	Sockbuf_init(&connp->r, sock, SERVER_RECV_SIZE, SOCKBUF_WRITE | SOCKBUF_READ | SOCKBUF_DGRAM);
	Sockbuf_init(&connp->c, -1, MAX_SOCKBUF_SIZE, SOCKBUF_WRITE | SOCKBUF_READ | SOCKBUF_LOCK);
	Sockbuf_init(&connp->q, -1, MAX_SOCKBUF_SIZE, SOCKBUF_WRITE | SOCKBUF_READ | SOCKBUF_LOCK);

	connp->my_port = my_port;
	connp->real = strdup(real);
	connp->nick = strdup(nick);
	connp->addr = strdup(addr);
	connp->host = strdup(host);
	connp->version = version;
	connp->start = turn;
	connp->magic = rand() + my_port + sock + turn;
	connp->id = -1;
	connp->timeout = LISTEN_TIMEOUT;
	connp->reliable_offset = 0;
	connp->reliable_unsent = 0;
	connp->last_send_loops = 0;
	connp->retransmit_at_loop = 0;
	connp->rtt_retransmit = DEFAULT_RETRANSMIT;
	connp->rtt_smoothed = 0;
	connp->rtt_dev = 0;
	connp->rtt_timeouts = 0;
	connp->acks = 0;
	connp->setup = 0;
	Conn_set_state(connp, CONN_LISTENING, CONN_FREE);
	if (connp->w.buf == NULL || connp->r.buf == NULL || connp->c.buf == NULL
		|| connp->q.buf == NULL || connp->real == NULL || connp->nick == NULL
		|| connp->addr == NULL || connp->host == NULL)
	{
		plog("Not enough memory for connection");
		Destroy_connection(free_conn_index, "no memory");
		return -1;
	}

	install_input(Handle_input, sock, free_conn_index);

	return my_port;
}

static int Handle_setup(int ind)
{
	connection_t *connp = &Conn[ind];
	char *buf;
	int n, len;
	
	if (connp->state != CONN_SETUP)
	{
		Destroy_connection(ind, "not setup");
		return -1;
	}

	if (connp->setup == 0)
	{
		n = Packet_printf(&connp->c, "%ld%hd",
			Setup.motd_len, Setup.frames_per_second);

		if (n <= 0)
		{
			Destroy_connection(ind, "Setup 0 write error");
			return -1;
		}

		connp->setup = (char *) &Setup.motd[0] - (char *) &Setup;
	}
	else if (connp->setup < Setup.setup_size)
	{
		if (connp->c.len > 0)
		{
			/* If there is still unacked reliable data test for acks. */
			Handle_input(-1, ind);
			if (connp->state == CONN_FREE)
				return -1;
		}
	}
	if (connp->setup < Setup.setup_size)
	{
		len = MIN(connp->c.size, 4096) - connp->c.len;
		if (len <= 0)
		{
			/* Wait for acknowledgement of previously transmitted data. */
			return 0;
		}
		if (len > Setup.setup_size - connp->setup)
			len = Setup.setup_size - connp->setup;

		buf = (char *) &Setup;
		if (Sockbuf_write(&connp->c, &buf[connp->setup], len) != len)
		{
			Destroy_connection(ind, "sockbuf write setup error");
			return -1;
		}
		connp->setup += len;
		if (len >= 512)
			connp->start += (len * FPS) / (8 * 512) + 1;
	}

	if (connp->setup >= Setup.setup_size)
		Conn_set_state(connp, CONN_DRAIN, CONN_LOGIN);

	return 0;
}


/*
 * Handle a connection that is in the listening state.
 */
static int Handle_listening(int ind)
{
	connection_t *connp = &Conn[ind];
	unsigned char type;
	int i, n;
	s16b sex, race, class;
	char nick[MAX_NAME_LEN], real[MAX_NAME_LEN], pass[MAX_NAME_LEN];

	if (connp->state != CONN_LISTENING)
	{
		Destroy_connection(ind, "not listening");
		return -1;
	}
	Sockbuf_clear(&connp->r);
	errno = 0;
	n = DgramReceiveAny(connp->r.sock, connp->r.buf, connp->r.size);

/*	s_printf("Listening on connection %d, received %d bytes.\n", ind, n);

	for (i = 0; i < n; i++)
	{
		printf("%3d ", connp->r.ptr[i] & 0xff);
		if (i % 20 == 0)
			printf("\n");
	}

	printf("\n"); */

	if (n <= 0)
	{
		if (n == 0 || errno == EWOULDBLOCK || errno == EAGAIN)
			n = 0;
		else if (n != 0)
			Destroy_connection(ind, "read first packet error");
		return n;
	}
	connp->r.len = n;
	connp->his_port = DgramLastport();
	if (DgramConnect(connp->w.sock, connp->addr, connp->his_port) == -1)
	{
		plog(format("Cannot connect datagram socket (%s,%d)",
			connp->host, connp->his_port));
		Destroy_connection(ind, "connect error");
		return -1;
	}
	s_printf("%s: Welcome %s=%s@%s (%s/%d)", showtime(), connp->nick,
		connp->real, connp->host, connp->addr, connp->his_port);
	if (connp->version != MY_VERSION)
		s_printf(" (version %04x)", connp->version);
	s_printf("\n");

	if (connp->r.ptr[0] != PKT_VERIFY)
	{
		Send_reply(ind, PKT_VERIFY, PKT_FAILURE);
		Send_reliable(ind);
		Destroy_connection(ind, "not connecting");
		return -1;
	}
	if ((n = Packet_scanf(&connp->r, "%c%s%s%s%hd%hd%hd", &type, real, nick, pass, &sex, &race, &class)) <= 0)
	{
		Send_reply(ind, PKT_VERIFY, PKT_FAILURE);
		Send_reliable(ind);
		Destroy_connection(ind, "verify incomplete");
		return -1;
	}

	/* Read the stat order */
	for (i = 0; i < 6; i++)
	{
		n = Packet_scanf(&connp->r, "%hd", &connp->stat_order[i]);

		if (n <= 0)
		{
			Destroy_connection(ind, "Misread stat order");
			return -1;
		}
	}

	/* Read the options */
	for (i = 0; i < 64; i++)
	{
		n = Packet_scanf(&connp->r, "%c", &connp->Client_setup.options[i]);

		if (n <= 0)
		{
			Destroy_connection(ind, "Misread options");
			return -1;
		}
	}

	/* Read the "unknown" char/attrs */
	for (i = 0; i < TV_MAX; i++)
	{
		n = Packet_scanf(&connp->r, "%c%c", &connp->Client_setup.u_attr[i], &connp->Client_setup.u_char[i]);

		if (n <= 0)
		{
			break;

#if 0
			Destroy_connection(ind, "Misread unknown redefinitions");
			return -1;
#endif
		}
	}

	/* Read the "feature" char/attrs */
	for (i = 0; i < MAX_F_IDX; i++)
	{
		n = Packet_scanf(&connp->r, "%c%c", &connp->Client_setup.f_attr[i], &connp->Client_setup.f_char[i]);

		if (n <= 0)
		{
			break;

#if 0
			Destroy_connection(ind, "Misread feature redefinitions");
			return -1;
#endif
		}
	}

	/* Read the "object" char/attrs */
	for (i = 0; i < MAX_K_IDX; i++)
	{
		n = Packet_scanf(&connp->r, "%c%c", &connp->Client_setup.k_attr[i], &connp->Client_setup.k_char[i]);

		if (n <= 0)
		{
			break;

#if 0
			Destroy_connection(ind, "Misread object redefinitions");
			return -1;
#endif
		}
	}

	/* Read the "monster" char/attrs */
	for (i = 0; i < MAX_R_IDX; i++)
	{
		n = Packet_scanf(&connp->r, "%c%c", &connp->Client_setup.r_attr[i], &connp->Client_setup.r_char[i]);

		if (n <= 0)
		{
			break;

#if 0
			Destroy_connection(ind, "Misread monster redefinitions");
			return -1;
#endif
		}
	}

	if (strcmp(real, connp->real))
	{
		s_printf("Client verified incorrectly (%s, %s)(%s, %s)",
			real, nick, connp->real, connp->nick);
		Send_reply(ind, PKT_VERIFY, PKT_FAILURE);
		Send_reliable(ind);
		Destroy_connection(ind, "verify incorrect");
		return -1;
	}
	
	/* Set his character info */
	connp->pass = strdup(pass);
	connp->sex = sex;
	connp->race = race;
	connp->class = class;

	Sockbuf_clear(&connp->w);
	if (Send_reply(ind, PKT_VERIFY, PKT_SUCCESS) == -1
		|| Packet_printf(&connp->c, "%c%u", PKT_MAGIC, connp->magic) <= 0
		|| Send_reliable(ind) <= 0)
	{
		Destroy_connection(ind, "confirm failed");
		return -1;
	}

	Conn_set_state(connp, CONN_DRAIN, CONN_SETUP);

	return -1;
}

/*
 * Sync the named options from the array of options.
 *
 * This is a crappy way of doing things....
 */
static void sync_options(int Ind)
{
	player_type *p_ptr = Players[Ind];

	/* Do the dirty work */
	p_ptr->carry_query_flag = p_ptr->options[3];
	p_ptr->use_old_target = p_ptr->options[4];
	p_ptr->always_pickup = p_ptr->options[5];
	p_ptr->stack_force_notes = p_ptr->options[8];
	p_ptr->stack_force_costs = p_ptr->options[9];
	p_ptr->find_ignore_stairs = p_ptr->options[16];
	p_ptr->find_ignore_doors = p_ptr->options[17];
	p_ptr->find_cut = p_ptr->options[18];
	p_ptr->find_examine = p_ptr->options[19];
	p_ptr->disturb_move = p_ptr->options[20];
	p_ptr->disturb_near = p_ptr->options[21];
	p_ptr->disturb_panel = p_ptr->options[22];
	p_ptr->disturb_state = p_ptr->options[23];
	p_ptr->disturb_minor = p_ptr->options[24];
	p_ptr->disturb_other = p_ptr->options[25];
	p_ptr->stack_allow_items = p_ptr->options[30];
	p_ptr->stack_allow_wands = p_ptr->options[31];
	p_ptr->view_perma_grids = p_ptr->options[34];
	p_ptr->view_torch_grids = p_ptr->options[35];
	p_ptr->view_reduce_lite = p_ptr->options[44];
	p_ptr->view_reduce_view = p_ptr->options[45];
	p_ptr->view_yellow_lite = p_ptr->options[56];
	p_ptr->view_bright_lite = p_ptr->options[57];
	p_ptr->view_granite_lite = p_ptr->options[58];
	p_ptr->view_special_lite = p_ptr->options[59];
}

/*
 * A client has requested to start active play.
 * See if we can allocate a player structure for it
 * and if this succeeds update the player information
 * to all connected players.
 */
static int Handle_login(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;
	int i;

	if (Id >= MAX_ID)
	{
		errno = 0;
		plog(format("Id too big (%d)", Id));
		return -1;
	}

	for (i = 1; i < NumPlayers + 1; i++)
	{
		if (strcasecmp(Players[i]->name, connp->nick) == 0)
		{
			errno = 0;
			plog(format("Name already in use %s", connp->nick));
			return -1;
		}
	}

	if (!player_birth(NumPlayers + 1, connp->nick, connp->pass, ind, connp->race, connp->class, connp->sex, connp->stat_order))
	{
		/* Failed, connection destroyed */
		return -1;
	}

	p_ptr = Players[NumPlayers + 1];
	strcpy(p_ptr->realname, connp->real);
	strcpy(p_ptr->hostname, connp->host);
	strcpy(p_ptr->addr, connp->addr);
	p_ptr->version = connp->version;

	/* Copy the client preferences to the player struct */
	for (i = 0; i < 64; i++)
	{
		p_ptr->options[i] = connp->Client_setup.options[i];
	}

	for (i = 0; i < TV_MAX; i++)
	{
		int j;

		if (!connp->Client_setup.u_attr[i] &&
		    !connp->Client_setup.u_char[i])
			continue;

		for (j = 0; j < MAX_K_IDX; j++)
		{
			if (k_info[j].tval == i)
			{
				p_ptr->d_attr[j] = connp->Client_setup.u_attr[i];
				p_ptr->d_char[j] = connp->Client_setup.u_char[i];
			}
		}
	}

	for (i = 0; i < MAX_F_IDX; i++)
	{
		p_ptr->f_attr[i] = connp->Client_setup.f_attr[i];
		p_ptr->f_char[i] = connp->Client_setup.f_char[i];

		if (!p_ptr->f_attr[i]) p_ptr->f_attr[i] = f_info[i].z_attr;
		if (!p_ptr->f_char[i]) p_ptr->f_char[i] = f_info[i].z_char;
	}

	for (i = 0; i < MAX_K_IDX; i++)
	{
		p_ptr->k_attr[i] = connp->Client_setup.k_attr[i];
		p_ptr->k_char[i] = connp->Client_setup.k_char[i];

		if (!p_ptr->k_attr[i]) p_ptr->k_attr[i] = k_info[i].x_attr;
		if (!p_ptr->k_char[i]) p_ptr->k_char[i] = k_info[i].x_char;

		if (!p_ptr->d_attr[i]) p_ptr->d_attr[i] = k_info[i].d_attr;
		if (!p_ptr->d_char[i]) p_ptr->d_char[i] = k_info[i].d_char;
	}

	for (i = 0; i < MAX_R_IDX; i++)
	{
		p_ptr->r_attr[i] = connp->Client_setup.r_attr[i];
		p_ptr->r_char[i] = connp->Client_setup.r_char[i];

		if (!p_ptr->r_attr[i]) p_ptr->r_attr[i] = r_info[i].x_attr;
		if (!p_ptr->r_char[i]) p_ptr->r_char[i] = r_info[i].x_char;
	}

	sync_options(NumPlayers + 1);

	GetInd[Id] = NumPlayers + 1;

	NumPlayers++;
	connp->id = Id++;
	
	Conn_set_state(connp, CONN_READY, CONN_PLAYING);

	if (Send_reply(ind, PKT_PLAY, PKT_SUCCESS) <= 0)
	{
		plog("Cannot send play reply");
		return -1;
	}
	
	/* Send party information */
	Send_party(NumPlayers);

	if (Send_reliable(ind) == -1)
	{
		Destroy_connection(ind, "Couldn't send reliable data");
		return -1;
	}

	num_logins++;

	/* If dungeon master, go no further */
	if (!strcmp(p_ptr->name,DUNGEON_MASTER)) return 0;

	/* Tell everyone about our new player */
	for (i = 1; i < NumPlayers; i++)
	{
		if (Players[i]->conn == NOT_CONNECTED)
			continue;

		msg_format(i, "%s has entered the game.", p_ptr->name);
	}

	/* Tell the meta server about the new player */
	Report_to_meta(META_UPDATE);

	return 0;
}


/*
 * Process a client packet.
 * The client may be in one of several states,
 * therefore we use function dispatch tables for easy processing.
 * Some functions may process requests from clients being
 * in different states.
 */
static void Handle_input(int fd, int arg)
{
	int ind = arg;
	connection_t *connp = &Conn[ind];
	int type, result, (**receive_tbl)(int ind);

	if (connp->state & (CONN_PLAYING | CONN_READY))
		receive_tbl = &playing_receive[0];
	else if (connp->state & (CONN_LOGIN/* | CONN_SETUP */))
		receive_tbl = &login_receive[0];
	else if (connp->state & (CONN_DRAIN/* | CONN_SETUP */))
		receive_tbl = &drain_receive[0];
	else if (connp->state == CONN_LISTENING)
	{
		Handle_listening(ind);
		return;
	}
	else if (connp->state == CONN_SETUP)
	{
		Handle_setup(ind);
		return;
	}
	else {
		if (connp->state != CONN_FREE)
			Destroy_connection(ind, "not input");
		return;
	}

	/* Sockbuf_clear(&connp->r); */
	
	/* Mega-Hack */
	if (connp->id != -1 && Players[GetInd[connp->id]]->new_level_flag) return;

	if (Sockbuf_read(&connp->r) == -1)
	{
		Destroy_connection(ind, "input error");
		return;
	}

	if (connp->q.len > 0)
	{
		if (connp->r.ptr > connp->r.buf)
			Sockbuf_advance(&connp->r, connp->r.ptr - connp->r.buf);
		if (Sockbuf_write(&connp->r, connp->q.ptr, connp->q.len) != connp->q.len)
		{
			errno = 0;
			Destroy_connection(ind, "Can't copy queued data to buffer");
			return;
		}

		connp->q.ptr += connp->q.len;
		Sockbuf_advance(&connp->q, connp->q.ptr - connp->q.buf);

		Sockbuf_clear(&connp->q);
	}

	if (connp->r.len <= 0)
		return;
	while (connp->r.ptr < connp->r.buf + connp->r.len)
	{
		type = (connp->r.ptr[0] & 0xFF);
		result = (*receive_tbl[type])(ind);
		if (connp->state == CONN_PLAYING)
		{
			connp->start = turn;
		}
		if (result == -1)
			return;
		if (result == 0)
		{
			Sockbuf_clear(&connp->r);
			break;
		}
		if (result == 2)
			return;
	}
	Sockbuf_clear(&connp->r);
}

int Net_input(void)
{
	int i, ind, num_reliable = 0, input_reliable[MAX_SELECT_FD];
	connection_t *connp;
	char msg[MSG_LEN];

	for (i = 0; i < max_connections; i++)
	{
		connp = &Conn[i];
		if (connp->state == CONN_FREE)
			continue;
		if (connp->start + connp->timeout * FPS < turn)
		{
			if (connp->state & (CONN_PLAYING | CONN_READY))
			{
				sprintf(msg, "%s mysteriously disappeared!",
					connp->nick);
				/*Set_message(msg);*/
			}
			sprintf(msg, "timeout %02x", connp->state);
			Destroy_connection(i, msg);
			continue;
		}

/*		if (connp->r.len > 0)
			Sockbuf_clear(&connp->r); */

#if 0
		if (connp->state != CONN_PLAYING)
		{
#endif
			input_reliable[num_reliable++] = i;
			if (connp->state == CONN_SETUP )
			{
				Handle_setup(i);
				continue;
			}
#if 0
		}
#endif
	}

	for (i = 0; i < num_reliable; i++)
	{
		ind = input_reliable[i];
		connp = &Conn[ind];
		if (connp->state & (CONN_DRAIN | CONN_READY | CONN_SETUP
			| CONN_LOGIN | CONN_PLAYING))
		{
			if (connp->c.len > 0)
				if (Send_reliable(ind) == -1)
					continue;
		}
	}

	if (num_logins | num_logouts)
		num_logins = num_logouts = 0;

	return login_in_progress;
}

int Net_output(void)
{
	int	i;
	connection_t *connp;
	player_type *p_ptr;

	for (i = 1; i <= NumPlayers; i++)
	{
		p_ptr = Players[i];

		if (p_ptr->conn == NOT_CONNECTED) continue;

		if (p_ptr->new_level_flag) continue;

		connp = &Conn[p_ptr->conn];

		/* XXX XXX XXX Mega-Hack -- Redraw player's spot every time */
		/* This keeps the network connection "active" even if nothing is */
		/* happening -- KLJ */
		/* This has been changed to happen less often, operating at close to 3 
		 * times a second to keep the BGP routing tables happy.  
		 * I had originally changed this to about once every 2 seconds, 
		 * but apparently it was doing bad things, as the inactivity in the UDP
		 * stream was causing us to loose priority in the routing tables.
		 * Thanks to Crimson for explaining this. -APD
		 */
		
		/* to keep a good UDP connection, send data that requests a response
		 * every 1/4 of a second
		 */

		/* Hack -- add the index to our turn, so we don't send all the players
		 * reliable data simultaniously.  This should hopefully "spread out"
		 * the incoming data a little so it doesn't all happen in a semi-
		 * synchronized way.
		 */

		if (!((turn + i) % 4)) 
		{
			lite_spot(i, p_ptr->py, p_ptr->px); 
			if (Send_reliable(p_ptr->conn) == -1)
				return -1;	
		}

		/* otherwise, send normal data if there is any */
		else 
		{
			 /* Tell the client that this is the end  */
			if (Packet_printf(&connp->w, "%c", PKT_END) <= 0)
			{
				Destroy_connection(p_ptr->conn, "write error");
				continue;
			}
			 /* Flush the output buffers */
			if (Sockbuf_flush(&connp->w) == -1)
				return -1;
		}

		Sockbuf_clear(&connp->w);
	}

	/* Every fifteen seconds, update the info sent to the metaserver */
	if (!(turn % (15 * FPS)))
		Report_to_meta(META_UPDATE);

	return 1;
}

/*
 * Send a reply to a special client request.
 * Not used consistently everywhere.
 * It could be used to setup some form of reliable
 * communication from the client to the server.
 */
int Send_reply(int ind, int replyto, int result)
{
	connection_t *connp = &Conn[ind];
	int n;

	n = Packet_printf(&connp->c, "%c%c%c", PKT_REPLY, replyto, result);
	if (n == -1)
	{
		Destroy_connection(ind, "write error");
		return -1;
	}

	return n;
}

int Send_leave(int ind, int id)
{
	connection_t *connp = &Conn[ind];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for leave info (%d,%d)",
			connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hd", PKT_LEAVE, id);
}

static int Receive_quit(int ind)
{
	int player, n, depth = 0;
	connection_t *connp = &Conn[ind];
	char ch;

	if (connp->id != -1) 
	{
		player = GetInd[connp->id];
		depth = Players[player]->dun_depth;
	}
	
	if ((n = Packet_scanf(&connp->r, "%c", &ch)) != 1)
	{
		errno = 0;
		Destroy_connection(ind, "receive error");
		return -1;
	}
	
	/* If we are close to the center of town, exit quickly. */
	if (depth <= 0 ? wild_info[depth].radius <= 2 : 0)
	{
		Destroy_connection(ind, "client quit");
		return -1;
	}
	return 1;
}

static int Receive_play(int ind)
{
	connection_t *connp = &Conn[ind];
	unsigned char ch;
	int n;

	if ((n = Packet_scanf(&connp->r, "%c", &ch)) != 1)
	{
		errno = 0;
		plog("Cannot receive play packet");
		Destroy_connection(ind, "receive error");
		return -1;
	}
	if (ch != PKT_PLAY)
	{
		errno = 0;
		plog("Packet is not of play type");
		Destroy_connection(ind, "not play");
		return -1;
	}
	if (connp->state != CONN_LOGIN)
	{
		if (connp->state != CONN_PLAYING)
		{
			if (connp->state == CONN_READY)
			{
				connp->r.ptr = connp->r.buf + connp->r.len;
				return 0;
			}
			errno = 0;
			plog(format("Connection not in login state (%02x)", connp->state));
			Destroy_connection(ind, "not login");
			return -1;
		}
		if (Send_reliable(ind) == -1)
			return -1;
		return 0;
	}

	Sockbuf_clear(&connp->w);
	if (Handle_login(ind) == -1)
	{
		/* The connection has already been destroyed */
		return -1;
	}

	return 2;
}

int Send_reliable(int ind)
{
	connection_t *connp = &Conn[ind];
	char *read_buf;
	int i, n, len, todo, max_todo;
	long rel_off;
	const int max_packet_size = MAX_RELIABLE_DATA_PACKET_SIZE,
		min_send_size = 1;

	if (connp->c.len <=0 || connp->last_send_loops == turn)
	{
		connp->last_send_loops = turn;
		return 0;
	}
	read_buf = connp->c.buf;
	max_todo = connp->c.len;
	rel_off = connp->reliable_offset;
	if (connp->w.len > 0)
	{
		if (connp->w.len >= max_packet_size - min_send_size)
		{
			return 0;
		}
		if (max_todo > max_packet_size - connp->w.len)
			max_todo = max_packet_size - connp->w.len;
	}
	if (connp->retransmit_at_loop > turn)
	{
		if (max_todo <= connp->reliable_unsent - connp->reliable_offset
			+ min_send_size || connp->w.len == 0)
			return 0;
	}
	else if (connp->retransmit_at_loop != 0)
		connp->acks >>= 1;

	todo = max_todo;

	for (i = 0; i <= connp->acks && todo > 0; i++)
	{
		len = (todo > max_packet_size) ? max_packet_size : todo;
		if (Packet_printf(&connp->w, "%c%hd%ld%ld%ld", PKT_RELIABLE,
			len, rel_off, turn, max_todo) <= 0
			|| Sockbuf_write(&connp->w, read_buf, len) != len)
		{
			plog("Cannot write reliable data");
			Destroy_connection(ind, "write error");
			return -1;
		}

		if ((n = Sockbuf_flush(&connp->w)) < len)
		{
			if (n == 0 && (errno == EWOULDBLOCK
				|| errno == EAGAIN))
			{
				connp->acks = 0;
				break;
			}
			else
			{
				plog(format("Cannot flush reliable data (%d)", n));
				Destroy_connection(ind, "flush error");
				return -1;
			}
		}

		todo -= len;
		rel_off += len;
		read_buf += len;
	}

	Sockbuf_clear(&connp->w);

	connp->last_send_loops = turn;

	if (max_todo - todo <= 0)
		return 0;

	if (connp->rtt_retransmit > MAX_RETRANSMIT)
		connp->rtt_retransmit = MAX_RETRANSMIT;
	if (connp->retransmit_at_loop <= turn)
	{
		connp->retransmit_at_loop = turn + connp->rtt_retransmit;
		connp->rtt_retransmit <<= 1;
		connp->rtt_timeouts++;
	}
	else connp->retransmit_at_loop = turn + connp->rtt_retransmit;

	if (rel_off > connp->reliable_unsent)
		connp->reliable_unsent = rel_off;

	return (max_todo - todo);
}

static int Receive_ack(int ind)
{
	connection_t *connp = &Conn[ind];
	int n;
	unsigned char ch;
	long rel, rtt, diff, delta, rel_loops;

	if ((n = Packet_scanf(&connp->r, "%c%ld%ld", &ch, &rel, &rel_loops))
		<= 0)
	{
		errno = 0;
		plog(format("Cannot read ack packet (%d)", n));
		Destroy_connection(ind, "read error");
		return -1;
	}
	if (ch != PKT_ACK)
	{
		errno = 0;
		plog(format("Not an ack packet (%d)", ch));
		Destroy_connection(ind, "not ack");
		return -1;
	}
	rtt = turn - rel_loops;
	if (rtt > 0 && rtt <= MAX_RTT)
	{
		if (connp->rtt_smoothed == 0)
			connp->rtt_smoothed = rtt << 3;
		delta = rtt - (connp->rtt_smoothed >> 3);
		connp->rtt_smoothed += delta;
		if (delta < 0)
			delta = -delta;
		connp->rtt_dev += delta - (connp->rtt_dev >> 2);
		connp->rtt_retransmit = ((connp->rtt_smoothed >> 2)
			+ connp->rtt_dev) >> 1;
		if (connp->rtt_retransmit < MIN_RETRANSMIT)
			connp->rtt_retransmit = MIN_RETRANSMIT;
	}
	diff = rel - connp->reliable_offset;
	if (diff > connp->c.len)
	{
		errno = 0;
		plog(format("Bad ack (diff=%ld,cru=%ld,c=%ld,len=%d)",
			diff, rel, connp->reliable_offset, connp->c.len));
		Destroy_connection(ind, "bad ack");
		return -1;
	}
	else if (diff <= 0)
		return 1;
	Sockbuf_advance(&connp->c, (int) diff);
	connp->reliable_offset += diff;
	if ((n = ((diff + 512 - 1) / 512)) > connp->acks)
		connp->acks = n;
	else
		connp->acks++;
	if (connp->reliable_offset >= connp->reliable_unsent)
	{
		connp->retransmit_at_loop = 0;
		if (connp->state == CONN_DRAIN)
			Conn_set_state(connp, connp->drain_state, connp->drain_state);
	}
	if (connp->state == CONN_READY
		&& (connp->c.len <= 0
		|| (connp->c.buf[0] != PKT_REPLY
			&& connp->c.buf[0] != PKT_PLAY
			&& connp->c.buf[0] != PKT_SUCCESS
			&& connp->c.buf[0] != PKT_FAILURE)))
		Conn_set_state(connp, connp->drain_state, connp->drain_state);
	
	connp->rtt_timeouts = 0;

	/*printf("Received ack to data sent at %ld.\n", rel_loops);*/

	return 1;
}
 
static int Receive_discard(int ind)
{
	connection_t *connp = &Conn[ind];

	errno = 0;
	plog(format("Discarding packet %d while in state %02x",
		connp->r.ptr[0], connp->state));
	connp->r.ptr = connp->r.buf + connp->r.len;

	return 0;
}

static int Receive_undefined(int ind)
{
	connection_t *connp = &Conn[ind];

	errno = 0;
	plog(format("Unknown packet type (%d,%02x)", connp->r.ptr[0], connp->state));
	Destroy_connection(ind, "undefined packet");
	return -1;
}

int Send_plusses(int ind, int tohit, int todam)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for plusses (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hd%hd", PKT_PLUSSES, tohit, todam);
}


int Send_ac(int ind, int base, int plus)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for ac (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hd%hd", PKT_AC, base, plus);
}

int Send_experience(int ind, int lev, int max, int cur, s32b adv)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for experience (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hu%d%d%d", PKT_EXPERIENCE, lev, 
			max, cur, adv);
}

int Send_gold(int ind, s32b au)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for gold (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%d", PKT_GOLD, au);
}

int Send_hp(int ind, int mhp, int chp)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for hp (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	return Packet_printf(&connp->c, "%c%hd%hd", PKT_HP, mhp, chp);
}

int Send_sp(int ind, int msp, int csp)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for sp (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hd%hd", PKT_SP, msp, csp);
}

int Send_char_info(int ind, int race, int class, int sex)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for char info (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hd%hd%hd", PKT_CHAR_INFO, race, class, sex);
}

int Send_various(int ind, int hgt, int wgt, int age, int sc)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for various (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hu%hu%hu%hu", PKT_VARIOUS, hgt, wgt, age, sc);
}

int Send_stat(int ind, int stat, int max, int cur)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for stat (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	return Packet_printf(&connp->c, "%c%c%hd%hd", PKT_STAT, stat, max, cur);
}

int Send_history(int ind, int line, cptr hist)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for history (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hu%s", PKT_HISTORY, line, hist);
}

int Send_inven(int ind, char pos, byte attr, int wgt, int amt, byte tval, cptr name)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for inven (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%c%c%hu%hd%c%s", PKT_INVEN, pos, attr, wgt, amt, tval, name);
}

int Send_equip(int ind, char pos, byte attr, int wgt, byte tval, cptr name)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for equip (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%c%c%hu%c%s", PKT_EQUIP, pos, attr, wgt, tval, name);
}

int Send_title(int ind, cptr title)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for title (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%s", PKT_TITLE, title);
}

int Send_depth(int ind, int depth)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for depth (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hu", PKT_DEPTH, depth);
}

int Send_food(int ind, int food)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for food (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hu", PKT_FOOD, food);
}

int Send_blind(int ind, bool blind)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for blind (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%c", PKT_BLIND, blind);
}

int Send_confused(int ind, bool confused)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for confusion (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%c", PKT_CONFUSED, confused);
}

int Send_fear(int ind, bool fear)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for fear (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%c", PKT_FEAR, fear);
}

int Send_poison(int ind, bool poisoned)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for poison (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%c", PKT_POISON, poisoned);
}

int Send_state(int ind, bool paralyzed, bool searching, bool resting)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for state (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hu%hu%hu", PKT_STATE, paralyzed, searching, resting);
}

int Send_speed(int ind, int speed)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for speed (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hd", PKT_SPEED, speed);
}

int Send_study(int ind, bool study)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for study (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%c", PKT_STUDY, study);
}

int Send_cut(int ind, int cut)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for cut (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hu", PKT_CUT, cut);
}

int Send_stun(int ind, int stun)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for stun (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hu", PKT_STUN, stun);
}

int Send_direction(int ind)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for direction (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c", PKT_DIRECTION);
}

int Send_message(int ind, cptr msg)
{
	connection_t *connp = &Conn[Players[ind]->conn];
	char buf[80];

	if (msg == NULL)
		return 1;

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for message (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	/* Clip end of msg if too long */
	strncpy(buf, msg, 78);
	buf[78] = '\0';

	return Packet_printf(&connp->c, "%c%s", PKT_MESSAGE, buf);
}

int Send_char(int ind, int x, int y, byte a, char c)
{
	if (!BIT(Conn[Players[ind]->conn].state, CONN_PLAYING | CONN_READY))
		return 0;

	return Packet_printf(&Conn[Players[ind]->conn].c, "%c%c%c%c%c", PKT_CHAR, x, y, a, c);
}

int Send_spell_info(int ind, int book, int i, cptr out_val)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for spell info (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c%hu%hu%s", PKT_SPELL_INFO, book, i, out_val);
}


int Send_item_request(int ind)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for item request (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c", PKT_ITEM);
}

int Send_flush(int ind)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for flush (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	return Packet_printf(&connp->c, "%c", PKT_FLUSH);
}

/*
 * As an attempt to lower bandwidth requirements, each line is run length
 * encoded.  Non-encoded grids are sent as normal, but if a grid is
 * repeated at least twice, then bit 0x40 of the attribute is set, and
 * the next byte contains the number of repetitions of the previous grid.
 */
int Send_line_info(int ind, int y)
{
	player_type *p_ptr = Players[ind];
	connection_t *connp = &Conn[p_ptr->conn];
	int x, x1, n;
	char c;
	byte a;

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for line info (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	
	/* Put a header on the packet */
	Packet_printf(&connp->c, "%c%hd", PKT_LINE_INFO, y);

	/* Each column */
	for (x = 0; x < 80; x++)
	{
		/* Obtain the char/attr pair */
		c = p_ptr->scr_info[y][x].c;
		a = p_ptr->scr_info[y][x].a;

		/* Start looking here */
		x1 = x + 1;

		/* Start with count of 1 */
		n = 1;

		/* Count repetitions of this grid */
		while (p_ptr->scr_info[y][x1].c == c &&
			p_ptr->scr_info[y][x1].a == a && x1 < 80)
		{
			/* Increment count and column */
			n++;
			x1++;
		}

		/* RLE if there at least 2 similar grids in a row */
		if (n >= 2)
		{
			/* Set bit 0x40 of a */
			a |= 0x40;

			/* Output the info */
			Packet_printf(&connp->c, "%c%c%c", c, a, n);

			/* Start again after the run */
			x = x1 - 1;
		}
		else
		{
			/* Normal, single grid */
			Packet_printf(&connp->c, "%c%c", c, a); 
		}
	}

	/* Hack -- Prevent buffer overruns by flushing after each line sent */
	/* Send_reliable(Players[ind]->conn); */
	
	return 1;
}

int Send_mini_map(int ind, int y)
{
	player_type *p_ptr = Players[ind];
	connection_t *connp = &Conn[p_ptr->conn];
	int x, x1, n;
	char c;
	byte a;

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for minimap (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}
	
	/* Packet header */
	Packet_printf(&connp->c, "%c%hd", PKT_MINI_MAP, y);

	/* Each column */
	for (x = 0; x < 80; x++)
	{
		/* Obtain the char/attr pair */
		c = p_ptr->scr_info[y][x].c;
		a = p_ptr->scr_info[y][x].a;

		/* Start looking here */
		x1 = x + 1;

		/* Start with count of 1 */
		n = 1;

		/* Count repetitions of this grid */
		while (p_ptr->scr_info[y][x1].c == c &&
			p_ptr->scr_info[y][x1].a == a && x1 < 80)
		{
			/* Increment count and column */
			n++;
			x1++;
		}

		/* RLE if there at least 2 similar grids in a row */
		if (n >= 2)
		{
			/* Set bit 0x40 of a */
			a |= 0x40;

			/* Output the info */
			Packet_printf(&connp->c, "%c%c%c", c, a, n);

			/* Start again after the run */
			x = x1 - 1;
		}
		else
		{
			/* Normal, single grid */
			Packet_printf(&connp->c, "%c%c", c, a); 
		}
	}

	/* Hack -- Prevent buffer overruns by flushing after each line sent */
	/* Send_reliable(Players[ind]->conn); */
	
	return 1;
}

int Send_store(int ind, char pos, byte attr, int wgt, int number, int price, cptr name)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for store item (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	return Packet_printf(&connp->c, "%c%c%c%hd%hd%d%s", PKT_STORE, pos, attr, wgt, number, price, name);
}

int Send_store_info(int ind, int num, int owner, int items)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for store info (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	return Packet_printf(&connp->c, "%c%hd%hd%hd", PKT_STORE_INFO, num, owner, items);
}

int Send_store_sell(int ind, int price)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for sell price (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	return Packet_printf(&connp->c, "%c%d", PKT_SELL, price);
}

int Send_target_info(int ind, int x, int y, cptr str)
{
	connection_t *connp = &Conn[Players[ind]->conn];
	char buf[80];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for target info (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	/* Copy */
	strncpy(buf, str, 79);

	/* Paranoia -- Add null */
	buf[79] = '\0';

	return Packet_printf(&connp->c, "%c%c%c%s", PKT_TARGET_INFO, x, y, buf);
}

int Send_sound(int ind, int sound)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for sound (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	return Packet_printf(&connp->c, "%c%c", PKT_SOUND, sound);
}

int Send_special_line(int ind, int max, int line, byte attr, cptr buf)
{
	connection_t *connp = &Conn[Players[ind]->conn];
	char temp[80];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for special line (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	strncpy(temp, buf, 79);
	temp[79] = '\0';
	return Packet_printf(&connp->c, "%c%hd%hd%c%s", PKT_SPECIAL_LINE, max, line, attr, temp);
}

int Send_floor(int ind, char tval)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for floor item (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	return Packet_printf(&connp->c, "%c%c", PKT_FLOOR, tval);
}

int Send_pickup_check(int ind, cptr buf)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for pickup check (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	return Packet_printf(&connp->c, "%c%s", PKT_PICKUP_CHECK, buf);
}

/* adding ultimate quick and dirty hack here so geraldo can play his 19th lvl char
   with the 80 character party name...... 
   -APD-
*/

int Send_party(int ind)
{
	player_type *p_ptr = Players[ind];
	connection_t *connp = &Conn[p_ptr->conn];
	char buf[160];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection nor ready for party info (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	sprintf(buf, "Party: %s", parties[p_ptr->party].name);

	/* MEGA hack */
	
	buf[60] = 0;

	if (p_ptr->party > 0)
	{
		strcat(buf, "     Owner: ");
		strcat(buf, parties[p_ptr->party].owner);
	}

	return Packet_printf(&connp->c, "%c%s", PKT_PARTY, buf);
}

int Send_special_other(int ind)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for special other (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	return Packet_printf(&connp->c, "%c", PKT_SPECIAL_OTHER);
}

int Send_skills(int ind)
{
	player_type *p_ptr = Players[ind];
	connection_t *connp = &Conn[p_ptr->conn];
	s16b skills[11];
	int i, tmp;
	object_type *o_ptr;

	/* Fighting skill */
	o_ptr = &p_ptr->inventory[INVEN_WIELD];
	tmp = p_ptr->to_h + o_ptr->to_h;
	skills[0] = p_ptr->skill_thn + (tmp * BTH_PLUS_ADJ);

	/* Shooting skill */
	o_ptr = &p_ptr->inventory[INVEN_BOW];
	tmp = p_ptr->to_h + o_ptr->to_h;
	skills[1] = p_ptr->skill_thb + (tmp * BTH_PLUS_ADJ);

	/* Basic abilities */
	skills[2] = p_ptr->skill_sav;
	skills[3] = p_ptr->skill_stl;
	skills[4] = p_ptr->skill_fos;
	skills[5] = p_ptr->skill_srh;
	skills[6] = p_ptr->skill_dis;
	skills[7] = p_ptr->skill_dev;

	/* Number of blows */
	skills[8] = p_ptr->num_blow;
	skills[9] = p_ptr->num_fire;

	/* Infravision */
	skills[10] = p_ptr->see_infra;

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for skills (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	Packet_printf(&connp->c, "%c", PKT_SKILLS);

	for (i = 0; i < 11; i++)
	{
		Packet_printf(&connp->c, "%hd", skills[i]);
	}

	return 1;
}
	

int Send_pause(int ind)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for skills (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	return Packet_printf(&connp->c, "%c", PKT_PAUSE);
}



int Send_monster_health(int ind, int num, byte attr)
{
	connection_t *connp = &Conn[Players[ind]->conn];

	if (!BIT(connp->state, CONN_PLAYING | CONN_READY))
	{
		errno = 0;
		plog(format("Connection not ready for monster health bar (%d.%d.%d)",
			ind, connp->state, connp->id));
		return 0;
	}

	return Packet_printf(&connp->c, "%c%c%c", PKT_MONSTER_HEALTH, num, attr);
}

/*
 * Return codes for the "Receive_XXX" functions are as follows:
 *
 * -1 --> Some error occured
 *  0 --> The action was queued (not enough energy)
 *  1 --> The action was ignored (not enough energy)
 *  2 --> The action completed successfully
 *
 *  Every code except for 1 will cause the input handler to stop
 *  processing actions.
 */
static int Receive_walk(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch, dir;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}
	else player = 0;

	if ((n = Packet_scanf(&connp->r, "%c%c", &ch, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	/* Disturb if running or resting */
	if (p_ptr->running || p_ptr->resting)
	{
		disturb(player, 0, 0);
		return 1;
	}

	/* Hack -- handle confusion */
	if (p_ptr->confused)
	{
		dir = 5;

		/* Prevent walking nowhere */
		while (dir == 5)
			dir = rand_int(9) + 1;
	}

	if (player && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_walk(player, dir, p_ptr->always_pickup);
		return 2;
	}

	return 1;
}

static int Receive_run(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	int n, player;
	char dir;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%c", &ch, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	/* Disturb if we want to change directions */
	if (dir != p_ptr->find_current) disturb(player, 0, 0);

	/* Hack -- Fix the running in '5' bug */
	if (dir == 5)
		return 1;

	/* APD added the energy check after the running direction has
	 * changed.
	 */

	if (connp->id != -1)
	{
		do_cmd_run(player, dir);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%c", ch, dir);
		return 0;
	}

	return 1;
}

static int Receive_tunnel(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch, dir;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%c", &ch, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_tunnel(player, dir);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%c", ch, dir);
		return 0;
	}

	return 1;
}

static int Receive_aim_wand(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch, dir;
	s16b item;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd%c", &ch, &item, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_aim_wand(player, item, dir);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%hd%c", ch, item, dir);
		return 0;
	}

	return 1;
}

static int Receive_drop(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	int n, player; 
	s16b item, amt;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd%hd", &ch, &item, &amt)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_drop(player, item, amt);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%hd%hd", ch, item, amt);
		return 0;
	}

	return 1;
}

static int Receive_fire(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch, dir = 5;

	int n, player;
	s16b item;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%c%hd", &ch, &dir, &item)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	/* Check confusion */
	if (p_ptr->confused)
	{
		/* Change firing direction */
		while (dir == 5)
			dir = randint(9) + 1;
	}

	if (connp->id != -1 && p_ptr->energy >= (level_speed(p_ptr->dun_depth) / p_ptr->num_fire))
	{
		do_cmd_fire(player, dir, item);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%c%hd", ch, dir, item);
		return 0;
	}

	return 1;
}

static int Receive_stand(int ind)
{
	connection_t *connp = &Conn[ind];

	char ch;

	int n, player;

	if (connp->id != -1) player = GetInd[connp->id];

	if ((n = Packet_scanf(&connp->r, "%c", &ch)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1)
	{
		do_cmd_stay(player, 1);
		return 2;
	}

	return -1;
}

static int Receive_destroy(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	s16b item, amt;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd%hd", &ch, &item, &amt)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_destroy(player, item, amt);
		return 2;
	}

	return 1;
}

static int Receive_look(int ind)
{
	connection_t *connp = &Conn[ind];

	char ch, dir;

	int n, player;

	if (connp->id != -1) player = GetInd[connp->id];

	if ((n = Packet_scanf(&connp->r, "%c%c", &ch, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1)
		do_cmd_look(player, dir);

	return 1;
}

static int Receive_spell(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	int n, player;

	s16b book, spell;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd%hd", &ch, &book, &spell)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_cast(player, book, spell);
		return 2;
	}
	else if (player)
	{
		p_ptr->current_spell = -1;
		Packet_printf(&connp->q, "%c%hd%hd", ch, book, spell);
		return 0;
	}

	return 1;
}

static int Receive_open(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch, dir;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%c", &ch, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_open(player, dir);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%c", ch, dir);
		return 0;
	}

	return 1;
}

static int Receive_pray(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	int n, player;

	s16b book, prayer;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd%hd", &ch, &book, &prayer)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_pray(player, book, prayer);
		return 2;
	}
	else if (player)
	{
		p_ptr->current_spell = -1;
		Packet_printf(&connp->q, "%c%hd%hd", ch, book, prayer);
		return 0;
	}

	return 1;
}

static int Receive_ghost(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	int n, player;

	s16b ability;

	if (connp->id != 1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &ability)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_ghost_power(player, ability);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%hd", ch, ability);
		return 0;
	}

	return 1;
}

static int Receive_quaff(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;
	s16b item;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &item)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_quaff_potion(player, item);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%hd", ch, item);
		return 0;
	}

	return 1;
}

static int Receive_read(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;
	s16b item;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &item)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_read_scroll(player, item);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%hd", ch, item);
		return 0;
	}

	return 1;
}

static int Receive_search(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c", &ch)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_search(player);
		return 2;
	}

	return 1;
}

static int Receive_take_off(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	s16b item;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &item)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_takeoff(player, item);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%hd", ch, item);
		return 0;
	}

	return 1;
}

static int Receive_use(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;
	s16b item;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &item)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_use_staff(player, item);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%hd", ch, item);
		return 0;
	}

	return 1;
}

static int Receive_throw(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch, dir;

	int n, player;
	s16b item;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%c%hd", &ch, &dir, &item)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_throw(player, dir, item);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%c%hd", ch, dir, item);
		return 0;
	}

	return 1;
}

static int Receive_wield(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	s16b item;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &item)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_wield(player, item);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%hd", ch, item);
		return 0;
	}

	return 1;
}

static int Receive_zap(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;
	s16b item;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &item)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_zap_rod(player, item);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%hd", ch, item);
		return 0;
	}

	return 1;
}

static int Receive_target(int ind)
{
	connection_t *connp = &Conn[ind];

	char ch;

	s16b dir;
	int n, player;

	if (connp->id != -1) player = GetInd[connp->id];

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1)
		do_cmd_target(player, dir);

	return 1;
}

static int Receive_target_friendly(int ind)
{
	connection_t *connp = &Conn[ind];

	char ch;

	s16b dir;
	int n, player;

	if (connp->id != -1) player = GetInd[connp->id];

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1)
		do_cmd_target_friendly(player, dir);

	return 1;
}


static int Receive_inscribe(int ind)
{
	connection_t *connp = &Conn[ind];

	char ch;

	int n, player;

	s16b item;

	char inscription[80];

	if (connp->id != -1) player = GetInd[connp->id];

	if ((n = Packet_scanf(&connp->r, "%c%hd%s", &ch, &item, inscription)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1)
		do_cmd_inscribe(player, item, inscription);

	return 1;
}

static int Receive_uninscribe(int ind)
{
	connection_t *connp = &Conn[ind];

	char ch;

	int n, player;

	s16b item;

	if (connp->id != -1) player = GetInd[connp->id];

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &item)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1)
		do_cmd_uninscribe(player, item);

	return 1;
}

static int Receive_activate(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	s16b item;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &item)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_activate(player, item);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%hd", ch, item);
		return 0;
	}

	return 1;
}

static int Receive_bash(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch, dir;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%c", &ch, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_bash(player, dir);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%c", ch, dir);
		return 0;
	}

	return 1;
}

static int Receive_disarm(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch, dir;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%c", &ch, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_disarm(player, dir);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%c", ch, dir);
		return 0;
	}

	return 1;
}

static int Receive_eat(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	s16b item;
	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &item)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_eat_food(player, item);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%hd", ch, item);
		return 0;
	}

	return 1;
}

		
static int Receive_fill(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;
	s16b item;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &item)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_refill(player, item);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%hd", ch, item);
		return 0;
	}

	return 1;
}

static int Receive_locate(int ind)
{
	connection_t *connp = &Conn[ind];

	char ch, dir;

	int n, player;

	if (connp->id != -1) player = GetInd[connp->id];

	if ((n = Packet_scanf(&connp->r, "%c%c", &ch, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1)
		do_cmd_locate(player, dir);

	return 1;
}

static int Receive_map(int ind)
{
	connection_t *connp = &Conn[ind];

	char ch;

	int n, player;

	if (connp->id != -1) player = GetInd[connp->id];

	if ((n = Packet_scanf(&connp->r, "%c", &ch)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1)
		do_cmd_view_map(player);

	return 1;
}

static int Receive_search_mode(int ind)
{
	connection_t *connp = &Conn[ind];

	char ch;

	int n, player;

	if (connp->id != -1) player = GetInd[connp->id];

	if ((n = Packet_scanf(&connp->r, "%c", &ch)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1)
		do_cmd_toggle_search(player);

	return 1;
}

static int Receive_close(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch, dir;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%c", &ch, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_close(player, dir);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%c", ch, dir);
		return 0;
	}

	return 1;
}

static int Receive_gain(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	int n, player;

	s16b book, spell;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%hd%hd", &ch, &book, &spell)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_study(player, book, spell);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%hd%hd", ch, book, spell);
		return 0;
	}

	return 1;
}

static int Receive_go_up(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c", &ch)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_go_up(player);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c", ch);
		return 0;
	}

	return 1;
}

static int Receive_go_down(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c", &ch)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_go_down(player);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c", ch);
		return 0;
	}

	return 1;
}


static int Receive_direction(int ind)
{
	connection_t *connp = &Conn[ind];

	char ch, dir;

	int n, player;

	if (connp->id != -1) player = GetInd[connp->id];
		else player = 0;

	if ((n = Packet_scanf(&connp->r, "%c%c", &ch, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1)
		Handle_direction(player, dir);

	return 1;
}

static int Receive_item(int ind)
{
	connection_t *connp = &Conn[ind];

	char ch;

	s16b item;

	int n, player;

	if (connp->id != -1) player = GetInd[connp->id];
		else player = 0;

	if ((n = Packet_scanf(&connp->r, "%c%hd", &ch, &item)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1)
		Handle_item(player, item);

	return 1;
}


static int Receive_message(int ind)
{
	connection_t *connp = &Conn[ind];

	char ch, buf[1024];

	int n, player;

	if (connp->id != -1) player = GetInd[connp->id];
		else player = 0;

	if ((n = Packet_scanf(&connp->r, "%c%S", &ch, buf)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	player_talk(player, buf);

	return 1;
}
	
static int Receive_purchase(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;
	int n, player;
	s16b item, amt;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}
	else player = 0;

	if ((n = Packet_scanf(&connp->r, "%c%hd%hd", &ch, &item, &amt)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (player && p_ptr->store_num > -1)
		store_purchase(player, item, amt);
	else if (player)
		do_cmd_purchase_house(player, item);

	return 1;
}

static int Receive_sell(int ind)
{
	connection_t *connp = &Conn[ind];

	char ch;
	int n, player;
	s16b item, amt;

	if (connp->id != -1) player = GetInd[connp->id];
		else player = 0;

	if ((n = Packet_scanf(&connp->r, "%c%hd%hd", &ch, &item, &amt)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (player)
		store_sell(player, item, amt);

	return 1;
}

static int Receive_store_leave(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch;
	int n, player;

	if (connp->id != -1) player = GetInd[connp->id];
		else player = 0;

	if (player)
		p_ptr = Players[player];
	
	if ((n = Packet_scanf(&connp->r, "%c", &ch)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (!player) return -1;

        /* Update stuff */
        p_ptr->update |= (PU_VIEW | PU_LITE);
        p_ptr->update |= (PU_MONSTERS);

        /* Redraw stuff */
        p_ptr->redraw |= (PR_WIPE | PR_BASIC | PR_EXTRA);

        /* Redraw map */
        p_ptr->redraw |= (PR_MAP);

        /* Window stuff */
        p_ptr->window |= (PW_OVERHEAD);

	/* Update store info */
	p_ptr->store_num = -1;

	return 1;
}

static int Receive_store_confirm(int ind)
{
	connection_t *connp = &Conn[ind];
	char ch;
	int n, player;

	if (connp->id != -1) player = GetInd[connp->id];
		else player = 0;

	if ((n = Packet_scanf(&connp->r, "%c", &ch)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (!player)
		return -1;

	store_confirm(player);

	return 1;
}

static int Receive_drop_gold(int ind)
{
	connection_t *connp = &Conn[ind];
	char ch;
	int n, player;
	s32b amt;

	if (connp->id != -1) player = GetInd[connp->id];
		else player = 0;

	if ((n = Packet_scanf(&connp->r, "%c%ld", &ch, &amt)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (!player)
		return 0;

	do_cmd_drop_gold(player, amt);

	return 1;
}

static int Receive_steal(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;

	char ch, dir;

	int n, player;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}

	if ((n = Packet_scanf(&connp->r, "%c%c", &ch, &dir)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (connp->id != -1 && p_ptr->energy >= level_speed(p_ptr->dun_depth))
	{
		do_cmd_steal(player, dir);
		return 2;
	}
	else if (player)
	{
		Packet_printf(&connp->q, "%c%c", ch, dir);
		return 0;
	}

	return 1;
}
	
static int Receive_redraw(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;
	int player, n;
	char ch;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}
	else player = 0;

	if ((n = Packet_scanf(&connp->r, "%c", &ch)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (player)
	{
		p_ptr->store_num = -1;
		p_ptr->redraw |= (PR_BASIC | PR_EXTRA | PR_MAP);
	}

	return 1;
}

static int Receive_rest(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;
	int player, n;
	char ch;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}
	else player = 0;

	if ((n = Packet_scanf(&connp->r, "%c", &ch)) <= 0)
	{
		if (n == -1)	
			Destroy_connection(ind, "read error");
		return n;
	}

	if (player)
	{
		/* Set flag */
		p_ptr->resting = TRUE;

		/* Redraw */
		p_ptr->redraw |= (PR_STATE);
	}

	return 1;
}

static int Receive_special_line(int ind)
{
	connection_t *connp = &Conn[ind];
	int player, n;
	char ch, type;
	s16b line;

	if (connp->id != -1) player = GetInd[connp->id];
		else player = 0;

	if ((n = Packet_scanf(&connp->r, "%c%c%hd", &ch, &type, &line)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (player)
	{
		switch (type)
		{
			case SPECIAL_FILE_NONE:
				Players[player]->special_file_type = FALSE;
				break;
			case SPECIAL_FILE_UNIQUE:
				do_cmd_check_uniques(player, line);
				break;
			case SPECIAL_FILE_ARTIFACT:
				do_cmd_check_artifacts(player, line);
				break;
			case SPECIAL_FILE_PLAYER:
				do_cmd_check_players(player, line);
				break;
			case SPECIAL_FILE_OTHER:
				do_cmd_check_other(player, line);
				break;
			case SPECIAL_FILE_SCORES:
				display_scores(player, line);
				break;
			case SPECIAL_FILE_HELP:
				do_cmd_help(player, line);
				break;
		}
	}

	return 1;
}

static int Receive_options(int ind)
{
	connection_t *connp = &Conn[ind];
	player_type *p_ptr;
	int player, i, n;
	char ch;

	if (connp->id != -1)
	{
		player = GetInd[connp->id];
		p_ptr = Players[player];
	}
	else
	{
		player = 0;
		p_ptr = NULL;
	}

	if ((n = Packet_scanf(&connp->r, "%c", &ch)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (player)
	{
		for (i = 0; i < 64; i++)
		{
			n = Packet_scanf(&connp->r, "%c", &p_ptr->options[i]);

			if (n <= 0)
			{
				Destroy_connection(ind, "read error");
				return n;
			}
		}

		/* Sync named options */
		sync_options(player);
	}

	return 1;
}

static int Receive_suicide(int ind)
{
	connection_t *connp = &Conn[ind];
	int player, n;
	char ch;

	if (connp->id != -1)
		player = GetInd[connp->id];
	else player = 0;

	if ((n = Packet_scanf(&connp->r, "%c", &ch)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	/* Commit suicide */
	do_cmd_suicide(player);

	return 1;
}

static int Receive_party(int ind)
{
	connection_t *connp = &Conn[ind];
	int player, n;
	char ch, buf[160];
	s16b command;

	if (connp->id != -1) player = GetInd[connp->id];
		else player = 0;

	if ((n = Packet_scanf(&connp->r, "%c%hd%s", &ch, &command, buf)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (player)
	{
		switch (command)
		{
			case PARTY_CREATE:
			{
				party_create(player, buf);
				break;
			}

			case PARTY_ADD:
			{
				party_add(player, buf);
				break;
			}

			case PARTY_DELETE:
			{
				party_remove(player, buf);
				break;
			}

			case PARTY_REMOVE_ME:
			{
				party_leave(player);
				break;
			}

			case PARTY_HOSTILE:
			{
				add_hostility(player, buf);
				break;
			}

			case PARTY_PEACE:
			{
				remove_hostility(player, buf);
				break;
			}
		}
	}

	return 1;
}

void Handle_direction(int Ind, int dir)
{
	player_type *p_ptr = Players[Ind];

	if (!dir)
	{
		p_ptr->current_spell = -1;
		p_ptr->current_rod = -1;
		p_ptr->current_activation = -1;
		return;
	}

	if (p_ptr->current_spell != -1)
	{
		if (p_ptr->ghost)
			do_cmd_ghost_power_aux(Ind, dir);
		else if (p_ptr->mp_ptr->spell_book == TV_MAGIC_BOOK)
			do_cmd_cast_aux(Ind, dir);
		else if (p_ptr->mp_ptr->spell_book == TV_PRAYER_BOOK)
			do_cmd_pray_aux(Ind, dir);
		else p_ptr->current_spell = -1;
	}
	else if (p_ptr->current_rod != -1)
		do_cmd_zap_rod_dir(Ind, dir);
	else if (p_ptr->current_activation != -1)
		do_cmd_activate_dir(Ind, dir);
}
		
void Handle_item(int Ind, int item)
{
	player_type *p_ptr = Players[Ind];
	int i;

	if ((p_ptr->current_enchant_h > 0) || (p_ptr->current_enchant_d > 0) ||
             (p_ptr->current_enchant_a > 0))
	{
		enchant_spell_aux(Ind, item, p_ptr->current_enchant_h,
			p_ptr->current_enchant_d, p_ptr->current_enchant_a);
	}
	else if (p_ptr->current_identify)
	{
		ident_spell_aux(Ind, item);
	}
	else if (p_ptr->current_star_identify)
	{
		identify_fully_item(Ind, item);
	}
	else if (p_ptr->current_recharge)
	{
		recharge_aux(Ind, item, p_ptr->current_recharge);
	}

	for (i = 0; i < INVEN_PACK; i++) inven_item_optimize(Ind, i);
}


/* receive a dungeon master command */
static int Receive_master(int ind)
{
	connection_t *connp = &Conn[ind];
	int player, n;
	char ch, buf[160];
	s16b command;

	if (connp->id != -1) player = GetInd[connp->id];
		else player = 0;

	/* Make sure this came from the dungeon master.  Note that it may be
	 * possobile to spoof this, so probably in the future more advanced
	 * authentication schemes will be neccecary. -APD
	 */

	if (strcmp(Players[player]->name, DUNGEON_MASTER)) return 0;

	if ((n = Packet_scanf(&connp->r, "%c%hd%s", &ch, &command, buf)) <= 0)
	{
		if (n == -1)
			Destroy_connection(ind, "read error");
		return n;
	}

	if (player)
	{
		switch (command)
		{
			case MASTER_LEVEL:
			{
				master_level(player, buf);
				break;
			}

			case MASTER_BUILD:
			{
				master_build(player, buf);
				break;
			}

			case MASTER_SUMMON:
			{
				master_summon(player, buf);
				break;
			}
		}
	}

	return 1;
}

/* automatic phase command, will try to phase door
 * in the best way possobile.
 *
 * This function should probably be improved a lot, I am just
 * doing a basic version for now.
 */

static int Receive_autophase(int Ind)
{
	player_type *p_ptr;
	connection_t *connp = &Conn[Ind];
	object_type *o_ptr;
	int player, n;

	if (connp->id != -1) player = GetInd[connp->id];
		else player = 0;

	/* a valid player was found, try to do the autophase */	
	if (player)
	{
		p_ptr = Players[Ind];
		/* first, check the inventory for phase scrolls */
		/* check every item of his inventory */
		for (n = 0; n < INVEN_PACK; n++)
		{
			o_ptr = &p_ptr->inventory[n];
			if ((o_ptr->tval == TV_SCROLL) && (o_ptr->sval == SV_SCROLL_PHASE_DOOR))
			{
				/* found a phase scroll, read it! */
				do_cmd_read_scroll(Ind, n);
				return 1;
			}
		}

		/* No scrolls, see if we can cast the phase door spell */
		/* Check for magic book I */

		for (n = 0; n < INVEN_PACK; n++)
		{
			o_ptr = &p_ptr->inventory[n];
			/* if this is the mage book I */
			if ((o_ptr->tval == TV_MAGIC_BOOK) && (o_ptr->sval == 0))
			{
				/* attempt to cast phase door */
				do_cmd_cast(Ind, 0, 2);
			}
		}
	}

	/* Failure!  We are in trouble... */

	return -1;
}

