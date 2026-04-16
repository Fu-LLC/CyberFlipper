import os
import discord
from discord.ext import commands
from github import Github

# Set these environment variables or replace with your values
discord_token = os.getenv('DISCORD_TOKEN')
github_token = os.getenv('GITHUB_TOKEN')
github_repo = os.getenv('GITHUB_REPO', 'Personfu/CYBERFLIPPER')
release_channel_id = int(os.getenv('DISCORD_RELEASE_CHANNEL_ID', '0'))  # Set your channel ID

intents = discord.Intents.default()
bot = commands.Bot(command_prefix='!', intents=intents)

g = Github(github_token)
repo = g.get_repo(github_repo)

@bot.event
async def on_ready():
    print(f'Logged in as {bot.user}')
    channel = bot.get_channel(release_channel_id)
    if channel:
        latest_release = repo.get_latest_release()
        msg = f'**New Release:** {latest_release.title}\n{latest_release.html_url}\n{latest_release.body}'
        await channel.send(msg)

# Command to manually check for new releases
@bot.command()
async def check_release(ctx):
    latest_release = repo.get_latest_release()
    msg = f'**Latest Release:** {latest_release.title}\n{latest_release.html_url}\n{latest_release.body}'
    await ctx.send(msg)

if __name__ == '__main__':
    bot.run(discord_token)
