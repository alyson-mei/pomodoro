# UI design documentation

## Timer layout

### Standard

```
┌──────────────────────────────────────────────────┐  
│                                                  │  <padding_header_vert>
│               COUNTDOWN: WORK (1 / 2)            │  header
│                                                  │  <padding_header_vert>
├──────────────────────────────────────────────────┤  
│                                                  │  <margin_after_header>
│                                                  │  
│                                                  │  
│                     00:55:70                     │  time
│            █░░░░░░░░░░░░░░░░░░░░░░░░   7%        │  progress_bar
│                                                  │  <margin_after_time>
│                      Coding,                     |  category
|	         Building a cozy Pomodoro app          │  activity
│                                                  │  <margin_after_category>
│                                                  │  
│                                                  │  
│             [Space] Pause   [Q] Quit             │  controls
│                                                  │  <margin_after_controls>
└──────────────────────────────────────────────────┘
<                      width                      >
<ph>							               <ph>
```

\* `ph` - horizontal padding
#### Strings

- Header: "{timer_mode}: {timer_work_mode}" + " ({cur_iter} / {total_iter})" * int(timer_mode == COUNTDOWN)

- Time: MM:SS:CC
  TODO: Switch to MM:SS:D
- Progress bar: "{bar}  {percentage}%"
  TODO: Check the consistency  

- Activity: "{category},\n" + "{activity}"

- Controls:
	1. ACTIVE: "\[Space] Pause    \[Q] Quit"
	2. PAUSED: "\[Space] Resume   \[Q] Quit"
	3. COMPLETED: "\[Enter] Continue \[Q] Quit"
	4. CANCELLED: "    \[Enter/Q]   Quit    "

#### Theme

Borders: MINIMAL, SINGLE, DOUBLE (see include/literals.c)
```c
static const Border minimal_borders[4] = {
	{"-", "-", "-"},
	{"|", " ", "|"},
	{"|", "-", "|"},
	{"-", "-", "-"}
};

static const Border single_borders[4] = {
	{"┌", "─", "┐"},
	{"│", " ", "│"},
	{"├", "─", "┤"},
	{"└", "─", "┘"}
};

static const Border double_borders[4] = {
	{"╔", "═", "╗"},
	{"║", " ", "║"},
	{"╠", "═", "╣"},
	{"╚", "═", "╝"}
};
```
Color theme: 
- MINIMAL
- SIMPLE
- CYBERPUNK
- FOREST
- SUNSET 


## Suggestions

### Timer layout: Minimal mode
```
WORK 1/4 │ 24:35 ███████░░░░░ 58% │ Coding │ [Space] Pause [Q] Quit
```

```
┌────────────────────┐
│                    │
│       24:35        │
│   ████████░░░░     │
│                    │
│    [Space] [Q]     │
│                    │
└────────────────────┘
```

```
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
 WORK 1/4  24:35  ████████░░░░░░░ 47%
 Coding → Building a cozy Pomodoro app
 [Space] Pause    [Q] Quit
━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

### Timer layout: Maximal mode

```
┌──────────────────────────────────────────────────────────┐
│                                                          │
│                 COUNTDOWN: WORK (1 / 4)                  │
│                     Session: Morning                     │
│                                                          │
├──────────────────────────────────────────────────────────┤
│                                                          │
│                                                          │
│                         24:35                            │
│              █████████████░░░░░░░░░░░░░  47%             │
│                   Started at 14:30                       │
│                    Ends at 15:00                         │
│                                                          │
│                        Coding,                           │
│              Building a cozy Pomodoro app                │
│                                                          │
│                  ─────────────────────                   │
│                                                          │
│              Today: 3 pomodoros completed                │
│                Focus time: 1h 15m                        │
│                  Current streak: 3                       │
│                                                          │
│                                                          │
│           [Space] Pause   [S] Skip   [Q] Quit            │
│                                                          │
│                                Last saved: 14:32         │
└──────────────────────────────────────────────────────────┘

┌─────────────────────────────┬─────────────────────┐
│                             │  TODAY              │
│   COUNTDOWN: WORK (1 / 4)   │  ✓ 3 pomodoros      │
│                             │  ⏱ 1h 15m           │
├─────────────────────────────┤  🔥 3 streak        │
│                             │                     │
│          24:35              │  NEXT UP            │
│   █████████░░░░░░░  47%     │  Short Break        │
│                             │  (5 minutes)        │
│         Coding,             │                     │
│  Building a cozy Pomodoro   ├─────────────────────┤
│           app               │                     │
│                             │  [Space] Pause      │
│  Started: 14:30             │  [S] Skip           │
│  Ends at: 15:00             │  [Q] Quit           │
│                             │                     │
└─────────────────────────────┴─────────────────────┘
```