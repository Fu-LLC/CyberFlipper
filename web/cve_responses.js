/*
  CyberFlipper CVE Response Engine
  Generates 1,400 educational CVE responses across 70 core topics and 20 understanding levels.
*/

const CVE_RESPONSE_LEVELS = [
  { index: 0, slug: 'grade1', label: 'First Grade', tone: 'simple', intro: 'A CVE is like a name for a computer boo-boo.' },
  { index: 1, slug: 'grade2', label: 'Second Grade', tone: 'simple', intro: 'A CVE is a special name for a computer bug that is dangerous.' },
  { index: 2, slug: 'grade3', label: 'Third Grade', tone: 'simple', intro: 'A CVE is a label for a bad software problem that people study.' },
  { index: 3, slug: 'grade4', label: 'Fourth Grade', tone: 'simple', intro: 'A CVE is a public name for a security bug that can hurt computers.' },
  { index: 4, slug: 'grade5', label: 'Fifth Grade', tone: 'simple', intro: 'A CVE is a numbered entry for a real computer security vulnerability.' },
  { index: 5, slug: 'middle-school', label: 'Middle School', tone: 'casual', intro: 'A CVE is a public identifier for a discovered software vulnerability.' },
  { index: 6, slug: 'high-school', label: 'High School', tone: 'conversational', intro: 'CVE stands for Common Vulnerabilities and Exposures, and it tags a security flaw.' },
  { index: 7, slug: 'intro-college', label: 'Intro College', tone: 'clear', intro: 'A CVE is a globally recognized identifier for a known vulnerability.' },
  { index: 8, slug: 'college', label: 'College', tone: 'informative', intro: 'CVEs are the public IDs used to track disclosed vulnerabilities.' },
  { index: 9, slug: 'advanced-college', label: 'Advanced College', tone: 'structured', intro: 'A CVE acts as a standard reference for a disclosed security issue.' },
  { index: 10, slug: 'early-pro', label: 'Early Professional', tone: 'practical', intro: 'A CVE gives a vulnerability a canonical identifier for tracking and response.' },
  { index: 11, slug: 'experienced-pro', label: 'Experienced Professional', tone: 'pragmatic', intro: 'CVEs are the common nomenclature used by security teams to reference known flaws.' },
  { index: 12, slug: 'senior-engineer', label: 'Senior Engineer', tone: 'focused', intro: 'A CVE is a structured identifier that allows cross-team collaboration on vulnerabilities.' },
  { index: 13, slug: 'architect', label: 'Security Architect', tone: 'contextual', intro: 'In vulnerability management, a CVE is the authoritative identifier for a disclosed issue.' },
  { index: 14, slug: 'red-team', label: 'Red Team Lead', tone: 'tactical', intro: 'A CVE encodes a publicly tracked vulnerability, often used in threat modeling and attack planning.' },
  { index: 15, slug: 'principal', label: 'Principal Researcher', tone: 'analytical', intro: 'A CVE provides a canonical tag for a vulnerability, allowing industry-wide coordination.' },
  { index: 16, slug: 'distinguished', label: 'Distinguished Researcher', tone: 'detailed', intro: 'A CVE is a normalized identifier issued for a documented security vulnerability.' },
  { index: 17, slug: 'phd-candidate', label: 'PhD Candidate', tone: 'academic', intro: 'A CVE is a standardized reference entry that captures disclosure metadata for a vulnerability.' },
  { index: 18, slug: 'phd', label: 'PhD', tone: 'scholarly', intro: 'A CVE is a canonical vulnerability identifier assigned within the MITRE-managed CVE Program.' },
  { index: 19, slug: '20yr-expert', label: '20-Year Expert', tone: 'expert', intro: 'A CVE is the established industry taxonomy for referencing a publicly disclosed vulnerability.' }
];

const CVE_RESPONSE_TOPICS = [
  {
    slug: 'what-is-cve',
    title: 'What is a CVE?',
    keywords: ['what is cve','define cve','cve meaning','what does cve mean','cve explain'],
    simple: 'A CVE is the public name given to a computer security problem.',
    technical: 'A CVE is a unique identifier assigned to a published vulnerability.',
    details: 'It is used by security teams to coordinate detection, patching, and reporting.',
    example: 'Example: CVE-2026-20131 is a vulnerability in Cisco Firepower Management Center.',
    advice: 'When you see a CVE, use it to look up related security advisories and mitigations.'
  },
  {
    slug: 'cve-vs-vulnerability',
    title: 'CVE vs Vulnerability',
    keywords: ['cve vs vulnerability','difference cve vulnerability','vulnerability not cve'],
    simple: 'A vulnerability is the bad thing; a CVE is the name we give it.',
    technical: 'A vulnerability is the actual flaw, while a CVE is the catalog entry for that flaw.',
    details: 'CVE entries standardize the way different teams refer to the same weakness.',
    example: 'For an SQL injection bug, the vulnerability is the bug, the CVE is its public ID.',
    advice: 'Use CVEs to avoid confusion when discussing specific vulnerabilities.'
  },
  {
    slug: 'cve-numbering',
    title: 'CVE Numbering and Syntax',
    keywords: ['cve numbering','cve syntax','cve format','how cve numbers work'],
    simple: 'CVE numbers look like CVE-YYYY-NNNNN and tell us when the bug was recorded.',
    technical: 'CVE IDs follow the format CVE-<year>-<sequence> and uniquely identify a disclosure.',
    details: 'The year is usually the publication year, and the sequence is the batch number.',
    example: 'CVE-2026-1340 refers to a vulnerability published in 2026.',
    advice: 'Read CVE numbers to quickly identify the disclosure year and reference the right advisory.'
  },
  {
    slug: 'cve-severity',
    title: 'CVE Severity and Risk',
    keywords: ['cve severity','cve risk','how severe a cve is','cve score'],
    simple: 'Severity is how bad the bug is for computers and people.',
    technical: 'Severity describes the potential impact of a vulnerability on systems and data.',
    details: 'Common severity labels are Critical, High, Medium, and Low.',
    example: 'A remote code execution bug is often rated Critical because it can let attackers run code.',
    advice: 'Use severity to prioritize which CVEs to fix first.'
  },
  {
    slug: 'cvss-score',
    title: 'CVSS Score Meaning',
    keywords: ['cvss score','what is cvss','cvss meaning','cvss severity'],
    simple: 'CVSS gives a number to show how serious a bug is.',
    technical: 'CVSS is a scoring system that quantifies vulnerability severity on a scale of 0 to 10.',
    details: 'It considers exploitability, impact, user interaction, and privileges required.',
    example: 'A 9.8 CVSS score means the vulnerability is very dangerous.',
    advice: 'Use CVSS scores with context; check how the score was calculated.'
  },
  {
    slug: 'nvd-cve',
    title: 'NVD and CVE Relationship',
    keywords: ['nvd cve','nvd vs cve','national vulnerability database','cve database'],
    simple: 'NVD is a website that keeps track of CVEs and details about them.',
    technical: 'NVD is the U.S. National Vulnerability Database that annotates CVE entries with scoring and metadata.',
    details: 'It enriches CVEs with CVSS, references, and impact metrics.',
    example: 'You can search CVE-2026-20131 in NVD to see the score and affected software.',
    advice: 'Use NVD to get additional technical context for CVEs.'
  },
  {
    slug: 'search-cve',
    title: 'How to Search the CVE Database',
    keywords: ['search cve','find cve','cve search','lookup cve'],
    simple: 'Type the CVE name into a search box on the CVE or NVD website.',
    technical: 'Use the CVE ID, vendor name, or product name to search relevant advisories.',
    details: 'Filtering by year, vendor, or severity helps narrow results quickly.',
    example: 'Search for “CVE-2026-20131” to find Cisco Firepower details.',
    advice: 'Always verify the search result matches the correct product and version.'
  },
  {
    slug: 'zero-day',
    title: 'What is a Zero-Day?',
    keywords: ['zero-day','zero day cve','what is zero day','zero-day vulnerability'],
    simple: 'A zero-day is a bug that is dangerous before anyone fixes it.',
    technical: 'A zero-day is a vulnerability that is exploited before a patch is publicly available.',
    details: 'It often receives rapid attention because attackers can use it immediately.',
    example: 'A zero-day CVE might be used in targeted attacks before vendors release updates.',
    advice: 'Treat zero-day exposures as top priority for detection and containment.'
  },
  {
    slug: 'exploitability',
    title: 'What is Exploitability?',
    keywords: ['exploitability','can it be exploited','exploit difficulty','exploitability score'],
    simple: 'Exploitability means how easy it is for a bad person to use the bug.',
    technical: 'Exploitability measures how feasible it is to craft an exploit for a vulnerability.',
    details: 'Factors include required privileges, user interaction, and attack complexity.',
    example: 'A remote service bug with no authentication is highly exploitable.',
    advice: 'Use exploitability to judge whether a CVE presents an immediate threat.'
  },
  {
    slug: 'remote-code-execution',
    title: 'Remote Code Execution (RCE)',
    keywords: ['rce','remote code execution','execute code remotely','rce cve'],
    simple: 'RCE is when a hacker can make a computer run commands from far away.',
    technical: 'Remote code execution is a vulnerability that allows attackers to run arbitrary code on a victim system remotely.',
    details: 'It is one of the most serious vulnerability classes because it can lead to full control.',
    example: 'An RCE bug in a web server can let an attacker take over the server.',
    advice: 'Patch RCE CVEs quickly and isolate exposed services.'
  },
  {
    slug: 'local-privilege-escalation',
    title: 'Local Privilege Escalation (LPE)',
    keywords: ['local privilege escalation','lpe','privilege escalation','local exploit'],
    simple: 'LPE is when a user can become an administrator on their own computer.',
    technical: 'Local privilege escalation allows a low-privileged user to gain higher privileges on the same host.',
    details: 'It is commonly used after initial access to achieve full system control.',
    example: 'A bug in a printer driver that lets a normal user become root is an LPE.',
    advice: 'Monitor local privilege changes and apply patches on workstations and servers.'
  },
  {
    slug: 'information-disclosure',
    title: 'Information Disclosure',
    keywords: ['information disclosure','data leak','info leak','cve leak'],
    simple: 'This type of bug lets someone see secrets they should not see.',
    technical: 'Information disclosure vulnerabilities expose sensitive data to unauthorized parties.',
    details: 'This may include credentials, source code, or private files.',
    example: 'An error page that reveals database passwords is an information disclosure bug.',
    advice: 'Encrypt sensitive data and validate error handling to reduce exposure.'
  },
  {
    slug: 'authentication-bypass',
    title: 'Authentication Bypass',
    keywords: ['authentication bypass','bypass login','auth bypass','bypass auth'],
    simple: 'It means someone can get in without using a password properly.',
    technical: 'Authentication bypass vulnerabilities allow attackers to access protected functionality without valid credentials.',
    details: 'These flaws often stem from broken logic or malformed input handling.',
    example: 'If a web form accepts any token, that is an auth bypass issue.',
    advice: 'Review authentication logic and ensure strict session validation.'
  },
  {
    slug: 'patch-management',
    title: 'Patch Management',
    keywords: ['patch management','patching','update software','apply patches'],
    simple: 'Patching means fixing the bug with a new update.',
    technical: 'Patch management is the process of deploying security updates to vulnerable systems.',
    details: 'It includes testing, scheduling, and tracking remediation progress.',
    example: 'Applying a vendor security update for a CVE is patch management.',
    advice: 'Keep a prioritized patch schedule and document each remediation step.'
  },
  {
    slug: 'responsible-disclosure',
    title: 'Responsible Disclosure',
    keywords: ['responsible disclosure','report vulnerability','disclosure process','vendor disclosure'],
    simple: 'Responsible disclosure means telling the right people before telling everyone else.',
    technical: 'It is the practice of privately reporting vulnerabilities to vendors before public disclosure.',
    details: 'This gives vendors time to prepare patches and protects users from active exploitation.',
    example: 'A researcher sends a CVE report to the software vendor and waits for a fix.',
    advice: 'Follow published disclosure policies and avoid leaking details prematurely.'
  },
  {
    slug: 'threat-actor',
    title: 'Threat Actor Exploitation',
    keywords: ['threat actor','exploit cve','attacker','cve exploitation'],
    simple: 'A threat actor is a bad person who uses bugs like CVEs to break things.',
    technical: 'Threat actors are adversaries who weaponize CVEs to compromise systems.',
    details: 'Understanding which actors target a CVE helps guide detection and response.',
    example: 'Nation-state actors may exploit a zero-day CVE in critical infrastructure.',
    advice: 'Map CVEs to likely threat actors when evaluating risk.'
  },
  {
    slug: 'cve-lifecycle',
    title: 'CVE Lifecycle',
    keywords: ['cve lifecycle','cve process','lifecycle','cve stages'],
    simple: 'A CVE goes from discovery to fixing and then telling everyone about it.',
    technical: 'The CVE lifecycle covers discovery, assignment, publication, and remediation.',
    details: 'It starts with reporting, includes advisory release, and ends with patch deployment.',
    example: 'A bug is found, assigned a CVE, documented, then patched and verified.',
    advice: 'Track CVE state across your remediation workflow.'
  },
  {
    slug: 'vendor-advisory',
    title: 'Vendor Advisories',
    keywords: ['vendor advisory','security advisory','vendor notice','advisory cve'],
    simple: 'A vendor advisory is the company telling you how to fix a CVE.',
    technical: 'Vendor advisories detail affected products, impact, and remediation steps for CVEs.',
    details: 'They are the authoritative source for patching and mitigation guidance.',
    example: 'Cisco publishes an advisory for each affected product when a CVE is assigned.',
    advice: 'Always follow the vendor advisory for the specific product version.'
  },
  {
    slug: 'mitigation-strategies',
    title: 'Mitigation Strategies',
    keywords: ['mitigation','workaround','reduce risk','cve mitigation'],
    simple: 'Mitigation means doing something to make the bug less dangerous.',
    technical: 'Mitigation strategies reduce risk when a patch is unavailable or delayed.',
    details: 'They can include configuration changes, access restrictions, and monitoring.',
    example: 'Disabling an exposed service is a mitigation for an unpatched CVE.',
    advice: 'Use mitigation until the vulnerability can be fully remediated.'
  },
  {
    slug: 'vulnerability-scanning',
    title: 'Vulnerability Scanning',
    keywords: ['vulnerability scanning','scan cve','scanner','scan vulnerabilities'],
    simple: 'Scanning is checking computers for known bad bugs like CVEs.',
    technical: 'Vulnerability scanning identifies assets affected by known CVEs and weak configurations.',
    details: 'Scanners compare system versions and configurations against CVE signatures.',
    example: 'A scan reveals that a server is vulnerable to a specific CVE ID.',
    advice: 'Run scans regularly and tune them to reduce false positives.'
  },
  {
    slug: 'risk-prioritization',
    title: 'Risk Prioritization',
    keywords: ['risk prioritization','prioritize cve','which cve first','cve priority'],
    simple: 'Prioritization means deciding which CVEs to fix first.',
    technical: 'Risk prioritization ranks CVEs by severity, exploitability, and business impact.',
    details: 'It balances technical risk with asset value and exposure.',
    example: 'A public-facing RCE CVE gets higher priority than a local low-risk bug.',
    advice: 'Create a patching plan based on attack likelihood and impact.'
  },
  {
    slug: 'attack-surface',
    title: 'Attack Surface',
    keywords: ['attack surface','surface area','cve exposure','attack vector'],
    simple: 'Attack surface is all the places a bad guy can try to break into your computer.',
    technical: 'Attack surface includes exposed services, user interfaces, and network paths vulnerable to CVEs.',
    details: 'Reducing attack surface limits the number of CVEs an attacker can reach.',
    example: 'Closing unused ports reduces exposure to network-facing CVEs.',
    advice: 'Harden systems and remove unnecessary services to shrink the attack surface.'
  },
  {
    slug: 'software-bill-of-materials',
    title: 'Software Bill of Materials (SBOM)',
    keywords: ['sbom','software bill of materials','third party components','dependency tracking'],
    simple: 'An SBOM is a list of all the pieces inside a program.',
    technical: 'An SBOM lists software components so CVEs can be traced to dependencies.',
    details: 'It helps identify vulnerable libraries and transitive dependencies quickly.',
    example: 'If a library in your app has a CVE, the SBOM tells you where it is used.',
    advice: 'Use SBOMs to map CVEs to your software supply chain.'
  },
  {
    slug: 'proof-of-concept',
    title: 'Proof of Concept (PoC)',
    keywords: ['proof of concept','poc exploit','exploit code','cve poc'],
    simple: 'A PoC is a simple example that shows the bug really works.',
    technical: 'A proof of concept demonstrates that a CVE is exploitable in practice.',
    details: 'PoC code may be used by researchers and, unfortunately, by malicious actors.',
    example: 'A PoC exploit might crash a vulnerable service or spawn a shell.',
    advice: 'Treat PoCs as indicators of real threat and prioritize remediation accordingly.'
  },
  {
    slug: 'exploit-kit',
    title: 'Exploit Kit',
    keywords: ['exploit kit','exploit kit cve','kit attack','automated exploit'],
    simple: 'An exploit kit is a toolbox that helps bad guys use CVEs automatically.',
    technical: 'Exploit kits bundle multiple CVEs and payloads for automated exploitation campaigns.',
    details: 'They often target widely deployed vulnerabilities for mass infection.',
    example: 'A web exploit kit can scan for a list of CVEs in browsers and servers.',
    advice: 'Block exploit kit traffic and patch common CVEs to reduce risk.'
  },
  {
    slug: 'supply-chain-risk',
    title: 'Supply Chain Risk',
    keywords: ['supply chain risk','software supply chain','cve supply chain','dependency risk'],
    simple: 'Supply chain risk means a bug in someone else’s code can hurt you too.',
    technical: 'Supply chain risk arises when third-party components with CVEs are included in your systems.',
    details: 'A vulnerable library can propagate CVE exposure across many products.',
    example: 'A CVE in an open source dependency may affect multiple applications at once.',
    advice: 'Track dependencies and patch or replace vulnerable components promptly.'
  },
  {
    slug: 'firmware-vulnerabilities',
    title: 'Firmware Vulnerabilities',
    keywords: ['firmware vulnerabilities','firmware cve','embedded device bug','firmware exploit'],
    simple: 'Firmware is low-level code in a device, and its bugs can be dangerous.',
    technical: 'Firmware vulnerabilities affect embedded systems and can be harder to patch than software bugs.',
    details: 'They often require special update tools or vendor firmware images.',
    example: 'A CVE in router firmware can allow attackers to control the device.',
    advice: 'Keep firmware up to date and validate vendor firmware sources.'
  },
  {
    slug: 'binary-hardening',
    title: 'Binary Hardening',
    keywords: ['binary hardening','hardening','cve mitigation','compiler hardening'],
    simple: 'Hardening makes code harder for bad guys to use when a CVE exists.',
    technical: 'Binary hardening adds protections like ASLR, DEP, and stack canaries to reduce exploitability.',
    details: 'These techniques can mitigate the impact of memory corruption CVEs.',
    example: 'A hardening-enabled binary is more resistant to buffer overflow exploits.',
    advice: 'Use modern compiler hardening options for critical software.'
  },
  {
    slug: 'reporting-cve',
    title: 'CVE Referencing in Reports',
    keywords: ['report cve','cve in report','reference cve','report vulnerability'],
    simple: 'Put the CVE number in your report so everyone knows which bug you mean.',
    technical: 'Referencing CVEs in reports gives readers a standard identifier for the issue.',
    details: 'Include CVE IDs alongside affected products and remediation actions.',
    example: 'Report: “CVE-2026-20131 affects Cisco FMC and requires patching.”',
    advice: 'Use CVE references to avoid ambiguity in vulnerability communication.'
  },
  {
    slug: 'cisa-kev',
    title: 'CISA KEV Feed',
    keywords: ['cisa kev','kev feed','known exploited vulnerabilities','cisa feed'],
    simple: 'The CISA KEV feed shows CVEs that attackers are using right now.',
    technical: 'The CISA Known Exploited Vulnerabilities feed highlights CVEs actively exploited in the wild.',
    details: 'It is used to prioritize emergency patching for high-risk vulnerabilities.',
    example: 'If a CVE is in KEV, it means it is a top priority for remediation.',
    advice: 'Monitor the KEV feed and patch listed CVEs immediately when possible.'
  },
  {
    slug: 'patch-tuesday',
    title: 'Patch Tuesday',
    keywords: ['patch tuesday','microsoft patch tuesday','tuesday patch','patch cycle'],
    simple: 'Patch Tuesday is when Microsoft releases a bunch of updates each month.',
    technical: 'Patch Tuesday refers to Microsoft’s monthly security update release schedule on the second Tuesday.',
    details: 'Many organizations coordinate their vulnerability management around it.',
    example: 'Microsoft patch Tuesday may include fixes for several CVEs in Windows and Office.',
    advice: 'Plan testing and deployment around patch Tuesday to maintain stability.'
  },
  {
    slug: 'cve-assignment',
    title: 'CVE Assignment Process',
    keywords: ['cve assignment','assign cve','mitre cve','cve request'],
    simple: 'Someone gives the bug a CVE number so everyone can talk about it easily.',
    technical: 'CVE assignment is the process of issuing a unique identifier for a reported vulnerability.',
    details: 'MITRE coordinates assignment requests from vendors, researchers, and CNA partners.',
    example: 'A researcher submits a bug, and MITRE assigns CVE-2026-1340 to it.',
    advice: 'If you discover a vulnerability, follow the appropriate CVE assignment workflow.'
  },
  {
    slug: 'cve-metadata',
    title: 'CVE Metadata Fields',
    keywords: ['cve metadata','metadata fields','cve data','cve info'],
    simple: 'CVE metadata is the extra details that describe the bug.',
    technical: 'Metadata includes descriptions, references, affected products, and publication dates.',
    details: 'Metadata helps analysts understand the scope and impact of a CVE.',
    example: 'A CVE record may include NVD links, issue descriptions, and vendor advisories.',
    advice: 'Review CVE metadata closely for accurate impact assessment.'
  },
  {
    slug: 'exploit-maturity',
    title: 'Exploit Maturity',
    keywords: ['exploit maturity','exploit stage','maturity cve','proof of concept'],
    simple: 'Exploit maturity shows whether a bug is just a story or already being used by attackers.',
    technical: 'Exploit maturity describes whether exploit code, proof of concept, or weaponized kits exist for a CVE.',
    details: 'Higher maturity means a greater likelihood of real-world exploitation.',
    example: 'A CVE with public exploit code is more mature than one only described in a report.',
    advice: 'Use exploit maturity to adjust urgency and monitoring.'
  },
  {
    slug: 'security-researcher',
    title: 'Security Researcher Role',
    keywords: ['security researcher','researcher cve','find vulnerabilities','cve discovery'],
    simple: 'A security researcher looks for CVEs and helps fix them.',
    technical: 'Researchers discover vulnerabilities and publish findings, often requesting CVE assignment.',
    details: 'Their work fuels patches and drives defensive improvements.',
    example: 'A researcher finds a critical bug and reports it to the vendor securely.',
    advice: 'Respect disclosure policies and document your findings clearly.'
  },
  {
    slug: 'patch-regression',
    title: 'Patch Regression Risk',
    keywords: ['patch regression','regression risk','patch breaks things','update causes issues'],
    simple: 'Sometimes a patch can fix one bug but cause another problem.',
    technical: 'Patch regression occurs when remediation introduces new defects or compatibility issues.',
    details: 'Testing helps catch regressions before broad deployment.',
    example: 'A security patch may break an application integration if not tested.',
    advice: 'Use staging environments and rollback plans for critical patches.'
  },
  {
    slug: 'configuration-vuln',
    title: 'Configuration Vulnerabilities',
    keywords: ['configuration vulnerabilities','misconfiguration','config cve','bad setup'],
    simple: 'A configuration vulnerability happens when something is set up wrong and becomes a security hole.',
    technical: 'Misconfigurations can create conditions equivalent to CVEs by exposing sensitive functionality.',
    details: 'Examples include default passwords, open ports, and unnecessary services.',
    example: 'Leaving a management interface accessible without authentication is a configuration vulnerability.',
    advice: 'Harden system configuration and follow vendor security guides.'
  },
  {
    slug: 'third-party-risk',
    title: 'Third-Party Software Risk',
    keywords: ['third-party risk','third party cve','dependency cve','vendor risk'],
    simple: 'If a program uses someone else’s code, bugs in their code can hurt you too.',
    technical: 'Third-party software risk arises when dependencies contain CVEs that affect your product.',
    details: 'Monitoring vendor components and library versions helps manage this risk.',
    example: 'A CVE in an open source library can impact many applications that use it.',
    advice: 'Track third-party CVEs and patch dependencies systematically.'
  },
  {
    slug: 'pentest-cve',
    title: 'Penetration Testing and CVEs',
    keywords: ['penetration testing','pentest cve','pen test cvss','attack based on cve'],
    simple: 'Pentesters use CVEs to test whether systems can be attacked safely.',
    technical: 'Penetration testers map CVEs to target systems and validate exploitability under controlled conditions.',
    details: 'They provide evidence that a CVE is exploitable in the customer environment.',
    example: 'A pentester may simulate a CVE attack against a web server to verify exposure.',
    advice: 'Use pentests to confirm remediation and improve defensive controls.'
  },
  {
    slug: 'sdlc',
    title: 'Secure Development Lifecycle (SDLC)',
    keywords: ['secure development lifecycle','sdlc','secure coding','cve prevention'],
    simple: 'SDLC means building software while thinking about security first, so there are fewer CVEs.',
    technical: 'A secure development lifecycle integrates vulnerability prevention and testing into software delivery.',
    details: 'It includes secure coding, code review, dependency scanning, and threat modeling.',
    example: 'Adding CVE scanning to build pipelines is part of SDLC maturity.',
    advice: 'Embed CVE awareness early in development to reduce post-release remediation.'
  },
  {
    slug: 'cve-embargo',
    title: 'CVE Embargo and Suppression',
    keywords: ['cve embargo','cve suppression','embargo cve','private cve'],
    simple: 'An embargo means keeping the bug secret for a while so it can be fixed first.',
    technical: 'CVE embargoing delays public disclosure to allow coordinated patching and mitigation.',
    details: 'Suppressed CVEs may remain private until the vendor is ready to publish.',
    example: 'A researcher may agree to embargo the CVE until the vendor releases a fix.',
    advice: 'Use embargoes responsibly and respect agreed disclosure timelines.'
  },
  {
    slug: 'vulnerability-management',
    title: 'Vulnerability Management Program',
    keywords: ['vulnerability management','vuln management program','vulnerability process','cve program'],
    simple: 'It is the process of finding, fixing, and tracking bugs like CVEs across your systems.',
    technical: 'A vulnerability management program encompasses CVE discovery, triage, remediation, and reporting.',
    details: 'It aligns security, IT, and operations around risk reduction.',
    example: 'A program may track CVEs from detection through patch deployment and verification.',
    advice: 'Build strong processes to keep CVE remediation organized and accountable.'
  },
  {
    slug: 'scan-frequency',
    title: 'CVE Scanning Frequency',
    keywords: ['scan frequency','scan cve frequency','how often scan','vulnerability scan schedule'],
    simple: 'Scanning often helps you find new bugs quickly before attackers do.',
    technical: 'Scan frequency determines how regularly you assess assets for new CVE exposure.',
    details: 'Daily or weekly scans are typical for critical infrastructure, while monthly scanning may suffice for lower-risk systems.',
    example: 'A weekly scan helps detect newly published CVEs affecting your environment.',
    advice: 'Increase scan frequency for internet-facing and high-value assets.'
  },
  {
    slug: 'public-disclosure',
    title: 'Public Disclosure Impact',
    keywords: ['public disclosure','cve disclosure','disclosure impact','announce cve'],
    simple: 'When a CVE is public, more people know about it, so it may become more dangerous.',
    technical: 'Public disclosure can increase exploitation pressure by exposing details to a wider audience.',
    details: 'Responsible disclosure aims to balance transparency with risk mitigation.',
    example: 'A widely publicized CVE can lead to rapid exploit development and scanning activity.',
    advice: 'Prepare response playbooks for disclosed high-risk CVEs.'
  },
  {
    slug: 'cve-triage',
    title: 'CVE Triage',
    keywords: ['cve triage','triage process','prioritize vulnerabilities','triage cve'],
    simple: 'Triage means sorting CVEs to decide which ones to fix first.',
    technical: 'CVE triage evaluates severity, asset exposure, and exploitability to assign remediation priority.',
    details: 'It helps teams allocate limited resources effectively.',
    example: 'A triage team may move a public RCE CVE to immediate action and schedule others later.',
    advice: 'Document triage decisions and revisit them as new information emerges.'
  },
  {
    slug: 'mitigation-vs-remediation',
    title: 'Mitigation vs Remediation',
    keywords: ['mitigation vs remediation','mitigation remediation','fix or mitigate','cve response'],
    simple: 'Mitigation makes the bug less dangerous; remediation fixes it fully.',
    technical: 'Mitigation reduces the impact of a CVE when a full fix is unavailable; remediation removes the flaw.',
    details: 'Mitigation may involve containment and workarounds, while remediation is typically patching or code change.',
    example: 'Blocking access to a vulnerable service is a mitigation; installing the patch is remediation.',
    advice: 'Use mitigation as a temporary measure and remediate as soon as possible.'
  },
  {
    slug: 'exploit-prediction',
    title: 'Exploit Prediction',
    keywords: ['exploit prediction','exploit likelihood','cve prediction','exploit chance'],
    simple: 'Exploit prediction is guessing whether a bug will be used by attackers.',
    technical: 'It involves estimating the probability that a CVE will be weaponized based on context and exposure.',
    details: 'Factors include vendor popularity, public proof-of-concept, and available attack surface.',
    example: 'A CVE in a widely used server product is more likely to be exploited.',
    advice: 'Prioritize CVEs with both high severity and high exploitation likelihood.'
  },
  {
    slug: 'threat-modeling',
    title: 'Threat Modeling',
    keywords: ['threat modeling','threat model cve','attack scenarios','risk model'],
    simple: 'Threat modeling is thinking about how a bad person might use a CVE against you.',
    technical: 'It is the process of mapping CVEs to attacker objectives, assets, and attack paths.',
    details: 'This helps identify which vulnerabilities matter most in your environment.',
    example: 'Modeling shows that a remote code CVE on an exposed server is a critical risk.',
    advice: 'Use threat modeling to prioritize remediation and detection efforts.'
  },
  {
    slug: 'embedded-cve',
    title: 'CVE in Embedded Devices',
    keywords: ['embedded cve','iot cve','device cve','embedded vulnerability'],
    simple: 'Embedded devices are small computers, and their bugs can also get CVE names.',
    technical: 'Embedded CVEs affect firmware and software running on IoT and industrial devices.',
    details: 'These devices often have limited patching options and long lifecycles.',
    example: 'A CVE in a router firmware component can impact network security.',
    advice: 'Inventory embedded devices and apply firmware updates where available.'
  },
  {
    slug: 'cve-reporting-requirements',
    title: 'CVE Reporting Requirements',
    keywords: ['cve reporting','report cve','cve requirements','submit cve'],
    simple: 'Reporting a CVE means telling the right people about the bug so it can get fixed.',
    technical: 'CVE reporting requires accurate information about the vulnerability, affected products, and references.',
    details: 'Researchers and vendors follow CNA or MITRE guidelines to request CVE assignment.',
    example: 'A complete report includes proof of concept, impact description, and reproducible steps.',
    advice: 'Ensure your CVE report is clear, verifiable, and mapped to the correct vendor product.'
  },
  {
    slug: 'zero-day',
    title: 'Zero-Day Vulnerabilities',
    keywords: ['zero day','zero-day','0 day','0-day','zero day vulnerability'],
    simple: 'A zero-day is a vulnerability that is known to attackers before a fix is available.',
    technical: 'Zero-day vulnerabilities are exploited before a patch is broadly released or systems are updated.',
    details: 'They pose high risk because defenders often have no prior signature or configuration guidance when attacks begin.',
    example: 'A zero-day in widely deployed networking software can be weaponized immediately after discovery.',
    advice: 'Harden exposed services, apply vendor mitigations quickly, and monitor for suspicious activity while vendors prepare fixes.'
  },
  {
    slug: 'ransomware',
    title: 'Most Dangerous CVEs',
    keywords: ['best cve','top cve','most dangerous cve','highest risk cve','worst cve'],
    simple: 'The most dangerous CVEs are those that are remote, unauthenticated, and actively exploited.',
    technical: 'Top risk CVEs combine high severity, public exploit code, and broad exposure across widely deployed systems.',
    details: 'These CVEs require fast patching and review because they often lead to ransomware, data theft, or full network compromise.',
    example: 'Critical remote code execution issues in internet-facing infrastructure are usually the highest priority.',
    advice: 'Start with critical RCE, authentication bypass, and widely deployed vendor CVEs when prioritizing remediation.'
  }
];

const CVE_RESPONSE_LIBRARY = [];

function buildCveResponse(subject, level) {
  const base = `${level.intro} ${subject.simple}`;
  const detail = `${subject.technical} ${subject.details}`;
  const scene = `${subject.example}`;
  const advice = `${subject.advice}`;
  if (level.index < 5) {
    return `${base} ${subject.example}`;
  }
  if (level.index < 10) {
    return `${base} ${detail} ${scene} ${advice}`;
  }
  if (level.index < 15) {
    return `${base} ${detail} ${scene} ${advice}`;
  }
  return `Expert explanation: ${base} ${detail} ${scene} ${advice}`;
}

function initCveResponseLibrary() {
  CVE_RESPONSE_LEVELS.forEach(level => {
    CVE_RESPONSE_TOPICS.forEach(subject => {
      const response = buildCveResponse(subject, level);
      CVE_RESPONSE_LIBRARY.push({
        id: `${subject.slug}-${level.slug}`,
        topic: subject.title,
        level: level.label,
        levelIndex: level.index,
        response,
        keywords: subject.keywords
      });
    });
  });
}

function normalizeText(text) {
  return text.toLowerCase().replace(/[^a-z0-9\s]/g, ' ').replace(/\s+/g, ' ').trim();
}

function findTopic(query) {
  const normalized = normalizeText(query);
  for (const subject of CVE_RESPONSE_TOPICS) {
    for (const keyword of subject.keywords) {
      if (normalized.includes(keyword)) {
        return subject;
      }
    }
  }
  return CVE_RESPONSE_TOPICS[0];
}

function parseLevel(query) {
  const normalized = normalizeText(query);
  if (normalized.includes('first grade') || normalized.includes('grade 1') || normalized.includes('kid') || normalized.includes('simple')) return 0;
  if (normalized.includes('second grade') || normalized.includes('grade 2')) return 1;
  if (normalized.includes('third grade') || normalized.includes('grade 3')) return 2;
  if (normalized.includes('fourth grade') || normalized.includes('grade 4')) return 3;
  if (normalized.includes('fifth grade') || normalized.includes('grade 5')) return 4;
  if (normalized.includes('middle school') || normalized.includes('easy')) return 5;
  if (normalized.includes('high school')) return 6;
  if (normalized.includes('intro college')) return 7;
  if (normalized.includes('college')) return 8;
  if (normalized.includes('advanced college') || normalized.includes('advanced')) return 9;
  if (normalized.includes('beginner') || normalized.includes('intro')) return 7;
  if (normalized.includes('professional') || normalized.includes('pro')) return 10;
  if (normalized.includes('experienced') || normalized.includes('seasoned')) return 11;
  if (normalized.includes('senior') || normalized.includes('architect')) return 13;
  if (normalized.includes('red team')) return 14;
  if (normalized.includes('principal') || normalized.includes('distinguished')) return 16;
  if (normalized.includes('phd candidate') || normalized.includes('phd candidate')) return 17;
  if (normalized.includes('phd') || normalized.includes('academic')) return 18;
  if (normalized.includes('20 years') || normalized.includes('20 year') || normalized.includes('expert')) return 19;
  return null;
}

function extractCveId(query) {
  const match = query.match(/(CVE-\d{4}-\d{4,7})/i);
  return match ? match[1].toUpperCase() : '';
}

function getCveDetailsById(cveId) {
  if (typeof CVE_DB !== 'undefined' && Array.isArray(CVE_DB)) {
    return CVE_DB.find(entry => entry.id.toUpperCase() === cveId.toUpperCase()) || null;
  }
  return null;
}

function buildCveSummary(cve, level) {
  const base = `${level.intro} ${cve.title}`;
  const detail = `${cve.desc}`;
  const scan = `This issue affects ${cve.vendor} ${cve.product} and is rated ${cve.sev}.`; 
  const advice = `Refer to the vendor advisory and apply the patch quickly. Use detection rules for ${cve.cwe}.`;
  if (level.index < 5) {
    return `${base} ${scan}`;
  }
  if (level.index < 10) {
    return `${base} ${detail} ${scan} ${advice}`;
  }
  if (level.index < 15) {
    return `${base} ${detail} ${scan} ${advice}`;
  }
  return `Expert summary: ${base} ${detail} ${scan} ${advice}`;
}

function getCuratedCveSet() {
  if (typeof CURATED_DB !== 'undefined' && Array.isArray(CURATED_DB) && CURATED_DB.length) {
    return CURATED_DB;
  }
  if (typeof CVE_DB !== 'undefined' && Array.isArray(CVE_DB)) {
    return CVE_DB;
  }
  return [];
}

function getTopCves(limit = 5) {
  const source = getCuratedCveSet();
  const rank = { CRITICAL: 1, HIGH: 2, MEDIUM: 3, LOW: 4 };
  return [...source]
    .sort((a, b) => (rank[a.sev] - rank[b.sev]) || (b.year - a.year))
    .slice(0, limit);
}

function getTopCveSummary(level) {
  const top = getTopCves(5);
  if (!top.length) {
    return buildCveResponse(CVE_RESPONSE_TOPICS.find(t => t.slug === 'best-cve') || CVE_RESPONSE_TOPICS[0], level);
  }
  const lines = top.map((c, index) => `${index + 1}. ${c.id} — ${c.vendor} ${c.product} (${c.sev})`).join('\n');
  return `${level.intro} Top risk CVEs are those that are remote, unauthenticated, and actively exploited. In this feed, the top candidates are:\n${lines}\nFocus on critical remote code execution and authentication bypass issues first.`;
}

function getRansomwareResponse(level) {
  const subject = CVE_RESPONSE_TOPICS.find(t => t.slug === 'ransomware');
  if (!subject) return buildCveResponse(CVE_RESPONSE_TOPICS[0], level);
  return buildCveResponse(subject, level);
}

function getSpecializedResponse(query, level) {
  const normalized = normalizeText(query);
  if (/(best|top|most dangerous|highest risk|worst)/.test(normalized) && normalized.includes('cve')) {
    return getTopCveSummary(level);
  }
  if (/ransomware/.test(normalized)) {
    return getRansomwareResponse(level);
  }
  if (/zero day|zero-day|0-day|0 day/.test(normalized)) {
    const subject = CVE_RESPONSE_TOPICS.find(t => t.slug === 'zero-day' || t.slug === 'what-is-cve');
    return buildCveResponse(subject || CVE_RESPONSE_TOPICS[0], level);
  }
  return null;
}

function getCveResponseForQuery(query) {
  const cveId = extractCveId(query);
  const levelIndex = parseLevel(query);
  const level = typeof levelIndex === 'number' ? CVE_RESPONSE_LEVELS[levelIndex] : CVE_RESPONSE_LEVELS[10];

  if (cveId) {
    const cve = getCveDetailsById(cveId);
    if (cve) {
      return buildCveSummary(cve, level);
    }
  }

  const special = getSpecializedResponse(query, level);
  if (special) {
    return special;
  }

  if (/patch|patch priority|priority|mitigation|fix|remediation/.test(query.toLowerCase())) {
    const subject = CVE_RESPONSE_TOPICS.find(topic => topic.slug === 'patch-management') || findTopic(query);
    return buildCveResponse(subject, level);
  }

  if (/attack|chain|exploit|rce|thread|threat/.test(query.toLowerCase())) {
    const subject = CVE_RESPONSE_TOPICS.find(topic => topic.slug === 'threat-actor') || findTopic(query);
    return buildCveResponse(subject, level);
  }

  return buildCveResponse(findTopic(query), level);
}

function getCveResponseById(id) {
  return CVE_RESPONSE_LIBRARY.find(entry => entry.id === id) || null;
}

function getRandomCveResponse() {
  const index = Math.floor(Math.random() * CVE_RESPONSE_LIBRARY.length);
  return CVE_RESPONSE_LIBRARY[index];
}

initCveResponseLibrary();

console.info(`Loaded CVE Response Engine with ${CVE_RESPONSE_LIBRARY.length} responses.`);
