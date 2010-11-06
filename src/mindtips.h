#ifdef JP
static cptr mind_tips[5][MAX_MIND_POWERS] =
{
{
	"近くの全ての見えるモンスターを感知する。レベル5で罠/扉、15で透明なモンスター、30で財宝とアイテムを感知できるようになる。レベル20で周辺の地形を感知し、45でその階全体を永久に照らし、ダンジョン内のすべてのアイテムを感知する。レベル25で一定時間テレパシーを得る。",
	"精神攻撃のビームまたは球を放つ。",
	"近距離のテレポートをする。",
	"遠距離のテレポートをする。",
	"レベル30未満で、モンスターを朦朧か混乱か恐怖させる球を放つ。レベル30以上で視界内の全てのモンスターを魅了する。抵抗されると無効。",
	"テレキネシスの球を放つ。",
	"一定時間、ACを上昇させる。レベルが上がると、酸、炎、冷気、電撃、毒の耐性も得られる。",
	"レベル25未満で、アイテムの雰囲気を知る。レベル25以上で、アイテムを鑑定する。",
	"レベル25未満で、自分を中心とした精神攻撃の球を発生させる。レベル25以上で、視界内の全てのモンスターに対して精神攻撃を行う。",
	"恐怖と朦朧から回復し、ヒーロー気分かつ加速状態でなければHPが少し回復する。さらに、一定時間ヒーロー気分になり、加速する。",
	"アイテムを自分の足元へ移動させる。",
	"精神攻撃の球を放つ。モンスターに命中すると、0〜1.5ターン消費する。抵抗されなければ、MPが回復する。",
	"無傷球をも切り裂く純粋なエネルギーのビームを放つ。",
	"時を止める。全MPを消費し、消費したMPに応じて長く時を止めていられる。",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
},
{
	"ごく小さい気の球を放つ。",
	"光源が照らしている範囲か部屋全体を永久に明るくする。",
	"一定時間、空中に浮けるようになる。",
	"射程の短い気のビームを放つ。",
	"一定時間、魔法防御能力を上昇させる。",
	"気を練る。気を練ると術の威力は上がり、持続時間は長くなる。練った気は時間とともに拡散する。練りすぎると暴走する危険がある。",
	"一定時間、攻撃してきた全てのモンスターを傷つけるオーラを纏う。",
	"隣りのモンスターに対して気をぶつけ、吹きとばす。",
	"大きな気の球を放つ。",
	"モンスター1体にかかった魔法を解除する。",
	"1体の幽霊を召喚する。",
	"自分を中心とした超巨大な炎の球を発生させる。",
	"射程の長い、強力な気のビームを放つ。",
	"しばらくの間、非常に速く動くことができる。",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
},
{
	"近くの思考することができるモンスターを感知する。",
	"攻撃した後、反対側に抜ける。",
	"トラップにかかるが、そのトラップを破壊する。",
	"周囲のダンジョンを揺らし、壁と床をランダムに入れ変える。",
	"全方向に向かって攻撃する。",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
},
{
	"近くの全てのモンスターを感知する。レベル15で透明なモンスターを感知する。レベル25で一定時間テレパシーを得る。レベル35で周辺の地形を感知する。全ての効果は、鏡の上でないとレベル4だけ余計に必要になる。",
	"自分のいる床の上に鏡を生成する。",
	"閃光の矢を放つ。レベル10以上では鏡の上で使うとビームになる。",
	"近距離のテレポートをする。",
	"自分の周囲や、 自分のいる部屋全体を明るくする。",
	"遠距離のテレポートをする。",
	"一定時間、鏡のオーラが付く。攻撃を受けると破片のダメージで反撃し、さらに鏡の上にいた場合近距離のテレポートをする。",
	"モンスターをテレポートさせるビームを放つ。抵抗されると無効。",
	"破片の球を放つ。",
	"全ての鏡の周りに眠りの球を発生させる。",
	"ターゲットに向かって魔力のビームを放つ。鏡に命中すると、その鏡を破壊し、別の鏡に向かって反射する。",
	"鏡の上のモンスターを消し去る。",
	"一定時間、ACを上昇させる。レベル32で反射が付く。レベル40で魔法防御が上がる。",
	"ターゲットに向かって強力な魔力のビームを放つ。鏡に命中すると、その鏡を破壊し、8方向に魔力のビームを発生させる。",
	"視界内のモンスターを減速させ、朦朧とさせ、混乱させ、恐怖させ、麻痺させる。鏡の上で使うと威力が高い。",
	"フロアを作り変える。鏡の上でしか使えない。",
	"短距離内の指定した場所にテレポートする。",
	"地上にいるときはダンジョンの最深階へ、ダンジョンにいるときは地上へと移動する。",
	"全ての攻撃が、1/2の確率で無効になる。",
	"視界内の2つの鏡とプレイヤーを頂点とする三角形の領域に、魔力の結界を発生させる。",
	"一定時間、ダメージを受けなくなるバリアを張る。切れた瞬間に少しターンを消費するので注意。",
},
{
	"半径3以内かその部屋を暗くする。",
	"近くの全ての見えるモンスターを感知する。レベル5で罠/扉/階段、レベル15でアイテムを感知できるようになる。レベル45でその階全体の地形と全てのアイテムを感知する。",
	"近距離のテレポートをする。",
	"攻撃を受けた瞬間にテレポートをするようになる。失敗するとその攻撃のダメージを受ける。テレポートに失敗することもある。",
	"遠距離のテレポートをする。",
	"攻撃してすぐにテレポートする。",
	"敵1体の動きを封じる。ユニークモンスター相手の場合又は抵抗された場合には無効。",
	"アイテムを識別する。",
	"一定時間、浮遊能力を得る。",
	"自分を中心とした火の球を発生させ、テレポートする。さらに、一定時間炎に対する耐性を得る。装備による耐性に累積する。",
	"素早く相手に近寄り攻撃する。",
	"ランダムな方向に8回くさびを投げる。",
	"敵を1体自分の近くに引き寄せる。",
	"ダメージのない混乱の球を放つ。",
	"1体のモンスターと位置を交換する。",
	"自分のいる床の上に、モンスターが通ると爆発してダメージを与えるルーンを描く。",
	"一定時間、半物質化し壁を通り抜けられるようになる。さらに、一定時間酸への耐性を得る。装備による耐性に累積する。",
	"自分を中心とした超巨大な毒、生命力吸収、混乱の球を発生させ、テレポートする。",
	"ランダムな方向に何回か炎か地獄かプラズマのビームを放つ。",
	"全ての攻撃が、1/2の確率で無効になる。",
	"",
},
{
	"TRANSLATE(Destroy an adjacent door.)",
	"TRANSLATE(Destroy an adjacent wall.)",
	"TRANSLATE(Attempts to reverse evolution for a single opponent.)",
	"TRANSLATE(Attempts to advance evolution for a single opponent.)",
	"TRANSLATE(Slow a single monster.)",
	"TRANSLATE(Eliminate monster offspring.)",
	"TRANSLATE(Haste yourself.)",
	"TRANSLATE(Haste a monster.)",
	"TRANSLATE(Slow all monsters in sight.)",
	"TRANSLATE(Imprison a single opponent in a time cage.)",
	"TRANSLATE(Temporal escape:  You flee to safety, but forget some of your recent experiences.)",
	"TRANSLATE(Restores life and stats.)",
	"TRANSLATE(Increases your melee and missile attacks for a bit.)",
	"時を止める。全MPを消費し、消費したMPに応じて長く時を止めていられる。",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
},
};
#else
static cptr mind_tips[MIND_MAX_CLASSES][MAX_MIND_POWERS] =
{
{
	"Detects visible monsters in your vicinity and more and more. Detects traps and doors at level 5, invisible monsters at level 15, items at level 30. And magic mapping at level 20. Lights and know the whole level at level 45. Gives telepathy at level 25.",
	"Fires a beam or ball which inflicts PSI damage.",
	"Teleport short distance.",
	"Teleport long distance.",
	"Stuns, confuses or scares a monster. Or attempts to charm all monsters in sight at level 30.",
	"Fires a ball which hurts monsters with telekinesis.",
	"Gives stone skin and some resistance to elements for a while. The level increased, the more number of resistances given.",
	"Gives feeling of an item. Or identify an item at level 25.",
	"Generate a ball centered on you which inflict monster with PSI damage. Or inflict all monsters with PSI damage at level 25.",
	"Removes fear and stun. Gives heroism and speed. Heals HP a little unless you already have heroism and temporal speed boost.",
	"Pulls a distant item close to you.",
	"Fires a ball which damages monsters and absorbs monsters' mind power. Absorbing is takes more turns which from 0 to 1.5.",
	"Fires a beam of pure energy which penetrate the invulnerability barrier.",
	"Stops time. Consumes all of your SP. The more consumes SP, the longer duration of spell.",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
},
{
	"Fires a very small energy ball.",
	"Lights up nearby area and the inside of a room permanently.",
	"Gives levitaion a while.",
	"Fires a short energy beam.",
	"Gives magic resistance for a while.",
	"Improves spirit energy power temporaly. Improved spirit energy will be more and more powerfull or have longer duration. Too many improving results in uncontrollable explosion of spirit energy.",
	"Gives aura which damages all monsters which attacked you for a while.",
	"Damages an adjacent monster, and blow it away.",
	"Fires a large energy ball.",
	"Dispels all magics which is effecting a monster.",
	"Summons ghosts.",
	"Generates a huge ball of flame centered on you.",
	"Fires a long, powerful energy beam.",
	"Gives extremely fast speed.",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
},
{
	"Detects all monsters except mindless in your vicinity.",
	"Attacks monster with your weapons normaly, then move through counter side of the monster.",
	"Sets off a trap, then destroy that trap.",
	"Shakes dungeon structure, and results in random swaping of floors and walls.",
	"Attacks all adjacent monsters.",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
},
{
	"Detects visible monsters in your vicinity and more and more. Detects invisible monsters at level 15. Gives telepathy at level 25. Magic mapping at level 35. All of effects need 4 more levels unless on a mirror.",
	"Makes a mirror under you.",
	"Fires bolt of flash light. Or fires beam of light on a mirror at level 10.",
	"Teleport short distance.",
	"Lights up nearby area and the inside of a room permanently.",
	"Teleport long distance.",
	"Gives aura of shards of mirror for a while. It cause counter attack to monsters which attacks you.",
	"Teleports all monsters on the line away unless resisted.",
	"Fires a ball of shards.",
	"Generate balls which send monsters to sleep on all mirrors in the whole level.",
	"Fires a beam of mana. If the beam hit a mirror, it breaks that mirror and reflects toward another mirror.",
	"Eliminates a monster on a mirror from current dungeon level.",
	"Gives bonus to AC. Gives reflection at level 32. Gives magic resistance at level 40.",
	"Fires a powerful beam of mana. If the beam hit a mirror, it breaks that mirror and fires 8 beams of mana to 8 different directions from that point.",
	"Attempts to slow, stun, confuse, scare, freeze all monsters in sight. Gets more power on a mirror.",
	"Recreates current dungeon level. Can only be used on a mirror.",
	"Teleport to given location.",
	"Recalls player from dungeon to town, or from town to the deepest level of dungeon.",
	"Completely protects you from any attacks at one in two chance.",
	"Generates a magical triangle which damages all monsters in the area. The vertices of the triangle is you and two mirrors in sight.",
	"Generates barrier which completly protect you from almost all damages. Takes a few your turns when the barrier breaks or duration time is exceeded.",
},

{
	"Darken nearby area and inside of a room.",
	"Detects visible monsters in your vicinity and more and more. Detects traps, doors and stairs at level 5, items at level 15. Lights and know the whole level at level 45.",
	"Teleport short distance.",
	"Teleport as you recieve an attack. Might be able to teleport just before recieveing damages at higher level.",
	"Teleport long distance.",
	"Attack and teleport in a time.",
	"Attempt to freeze a monster.",
	"Identifies an item.",
	"Gives levitation for a while.",
	"Generate a fire ball and teleport in a time. Gives resistance to fire for a while. This resistance can be added to which from equipment for more powerful resistance.",
	"Steps close to a monster and attacks at a time.",
	"Shoots 8 iron Spikes in 8 random directions.",
	"Teleport a monster to a place adjacent to you.",
	"Releases a confusion ball which doesn't inflict any damage.",
	"Swaps positions of you and a monster.",
	"Sets a glyph under you. The glyph will explode when a monster moves on it.",
	"Become ethereal form which gives ability to pass walls, and gives resistance to acid for a while. This resistance can be added to which from equipment for more powerful resistance.",
	"Generates huge balls of poison, drain life and confusion, then teleport in a time.",
	"Fires some number of beams of fire, nether or plasma in random directions.",
	"Creates shadows of yourself which gives you abillity to completely evade any attacks at one in two chance for a while.",
	"",
},
{
	"Destroy an adjacent door.",
	"Destroy an adjacent wall.",
	"Attempts to reverse evolution for a single opponent.",
	"Attempts to advance evolution for a single opponent.",
	"Slow a single monster.",
	"Eliminate monster offspring.",
	"Haste yourself.",
	"Haste a monster.",
	"Slow all monsters in sight.",
	"Imprison a single opponent in a time cage.",
	"Temporal escape:  You flee to safety, but forget some of your recent experiences.",
	"Restores life and stats.",
	"Increases your melee and missile attacks for a bit.",
	"Stops time. Consumes all of your SP, and the more SP consumed, the longer the duration of the spell.",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
},
{
	"Cuts yourself.",
	"Detects living creatures in the vicinity",
	"Cuts yourself, splattering nearby enemies.",
	"Restores constitution and cures poison.",
	"Gives bonus to AC depending on how wounded you are.  Grants reflection if you are really hurting.",
	"Gives slay living to your weapon.",
	"Enter a blood frenzy.  Gives speed and big bonuses to hit and damage, but you can no longer think clearly.",
	"Whenever damaged by a living monster, it is injured by the same amount.",
	"You begin to feast on your opponents blood, doing extra damage but at a cost to your own health.",
	"Creates a macabre Potion of Healing made of your own blood.",
	"Damages all living creatures in sight at tremendous cost to your own health.",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
	"",
},
};
#endif

